//Prevent miniwindef.h from defining min/max (For WaitForSockets.h)
#define NOMINMAX

#include <StdUtils/Networking/SocketMonitor.h>


#include <StdUtils/Exceptions.h>
#include <StdUtils/Networking/TcpSocket.h>
#include <private/Networking/WaitForSockets.h>

using namespace std;

using namespace StdUtils::Networking;
using namespace StdUtils::Threading;
using namespace StdUtils::Private;
using namespace StdUtils::Private::Networking;


namespace StdUtils
{
	namespace Networking
	{
		static const char UpdateSignal(1);
		static const char CancelSignal(2);
		static const char DestructionSignal(3);

		SocketMonitor::SocketMonitor()
			:waiting(false)
		{
			this->updateSignals.Signaled = false;
		}
		SocketMonitor::~SocketMonitor()
		{
			STDUTILS_SYNCHRONIZE(this->interfaceLock);
			if (this->waiting)
				this->signalUpdate(DestructionSignal);
		}

		void SocketMonitor::initializeSignals()
		{
			if (!this->updateSignals.Signaler.isConnected())
			{
				this->updateSignals.Signaled = false;

				//TODO: Evtl. bessere Loesung?
				//Unbound udp socket laeuft nicht; Select gibt wegen der immer augenblicklich zurueck und ausserdem ist das Schliessen nicht threadsafe, wenns aus einem anderen Thread kommt
				//Diese 2 Sockets sind supi, aber sie belegen, zumindest waehrend des Verbindungsaufbaus, einen random port. Das ist echt unschoen.
				TcpSocket sock;
				sock.bind(Endpoint::AnyPort, IPAddress::Localhost(AddressTypes::IPv4));
				sock.listen(1);

				this->updateSignals.Signaler.connect(sock.getLocalEndpoint());

				sock.accept(this->updateSignals.Signalee);

				sock.close();
			}
		}


		/*
		Status when called:
		interfaceLock taken
		listLock released
		semaphore lock not acquired
		*/
		void SocketMonitor::signalUpdate(char type)
		{
			this->semaphore.acquireLock();

			if (this->waiting)
			{
				//=> waitForUpdates-Thread is right before, in, or right after WaitForIO call => It WILL respond to signals I send here, even if WaitForIO has already returned (it goes by updateSignals.Signaled-Flag
#ifdef _DEBUG
				//Debug-Mode-Exception, because the interfaceLock should prevent signalUpdate being called twice at the same time
				//That plus the last comment about the current execution position of waitForUpdates equals some seriously wrong shit
				if (this->updateSignals.Signaled)
				{
					this->semaphore.releaseLock();
					throw Exception("DEBUG SocketMonitor::signalUpdate: SocketMonitor waitForUpdates-Thread is already signaled");
				}
#endif
				try
				{
					this->updateSignals.Signaler.send(&type, sizeof(type));
				}
				catch (...)
				{
					this->semaphore.releaseLock();
					throw; //TODO: Wrapper-Exception with some abstracted info (the rethrown exception here will have originated from TcpSocket::send, therefore the actual position of the problem will not be easily deducible)
				}
				this->updateSignals.Signaled = true;

				this->semaphore.wait(); //releases semaphore-lock until it is signaled, reacquires it before returning
			}

			this->semaphore.releaseLock();
		}

		void SocketMonitor::set(Socket<SocketTypes::Any>* monitorable, bool input, bool output, void* customData)
		{
			STDUTILS_SYNCHRONIZE(this->interfaceLock);
			this->listLock.lock();

			try
			{
				vector<MonitoredSocket>::iterator it;
				for (it = this->monitored.begin(); it != this->monitored.end(); it++)
				{
					if (it->Instance == monitorable)
					{
						bool inChanged = input != it->MonitorInput;
						bool outChanged = output != it->MonitorOutput;

						if (input || output)
						{
							//Must happen before changing anything, because the monitorIn/monitorOut-methods throw, if the IOMonitorable is already monitored
							//Also, they ensure that, once passed, no other SocketMonitor may start monitoring this IOMonitorable
							//TODO: Bessere Loesung fuer workaround-try-catch in if (outChanged)
							if (inChanged)
								monitorable->monitorIn(input ? SocketStatusMonitored : SocketStatusNone);
							if (outChanged)
							{
								try
								{
									monitorable->monitorOut(output ? SocketStatusMonitored : SocketStatusNone);
								}
								catch (...)
								{
									if (inChanged)
										monitorable->monitorIn(input ? SocketStatusNone : SocketStatusMonitored);
									throw;
								}
							}

							it->MonitorInput = input;
							it->MonitorOutput = output;
							it->CustomData = customData;

							this->listLock.release();
							this->signalUpdate(UpdateSignal);
						}
						else
						{
							//Erase is handled in waitForUpdate

							it->MonitorInput = input;
							it->MonitorOutput = output;
							it->CustomData = customData;

							this->listLock.release();
							this->signalUpdate(UpdateSignal);

							//Must happen AFTER signalUpdate, to be sure the handles are not monitored any more.
							//Also to make sure another SocketMonitor is not allowed to monitor the IOMonitorable until this instance is definitely done with it.
							//The monitorIn/monitorOut calls should not throw exceptions here, unless something has gone horribly wrong
							if (inChanged)
								monitorable->monitorIn(input ? SocketStatusMonitored : SocketStatusNone);
							if (outChanged)
								monitorable->monitorOut(output ? SocketStatusMonitored : SocketStatusNone);
						}

						return;
					}
				}

				//If the IOMonitorable was not found in the list:
				if (input)
					monitorable->monitorIn(SocketStatusMonitored);
				else if (!output)
				{
					this->listLock.release();
					return;
				}

				//NOT else
				if (output)
					monitorable->monitorOut(SocketStatusMonitored);

				this->monitored.push_back(MonitoredSocket());
				it = this->monitored.end() - 1;
				MonitoredSocket& m = *it;

				m.Instance = monitorable;
				m.MonitorInput = input;
				m.MonitorOutput = output;
				m.CustomData = customData;

				this->listLock.release();
				this->signalUpdate(UpdateSignal);
			}
			catch (...)
			{
				this->listLock.release();
				throw;
			}
		}
		void SocketMonitor::remove(Socket<SocketTypes::Any>* s)
		{
			this->set(s, false, false, NULL);
		}

		SocketMonitor::size_type SocketMonitor::getMonitoredCount() const
		{
			return this->monitored.size(); //TODO: Lock? (const issue)
		}

		SocketUpdates SocketMonitor::waitForUpdate(uint32_t timeoutMS)
		{
			SocketUpdates rv;

			this->semaphore.acquireLock();

			try
			{
				if (this->waiting)
					throw Exception("A different thread is already waiting on this SocketMonitor");
				if (this->monitored.size() == 0)
					throw Exception("There are no handles to be monitored");

				this->initializeSignals();

				this->waiting = true;


				bool retry;
				do
				{
					retry = false;

					SocketWaitData* nodes;
					uint32_t nodeCount;

					STDUTILS_LOCK(this->listLock)
					{
						if (this->monitored.size() == 0)
							break;

						nodeCount = (uint32_t)(this->monitored.size() + 1);
						nodes = new SocketWaitData[nodeCount];
						nodes[0].H = this->updateSignals.Signalee.getHandle();
						nodes[0].Input = true;
						nodes[0].Output = false;

						int i = 1;
						for (vector<MonitoredSocket>::iterator it = this->monitored.begin(); it != this->monitored.end();)
						{
							if (it->MonitorInput || it->MonitorOutput)
							{
								if (it->MonitorInput)
									it->Instance->monitorIn(SocketStatusActivelyMonitored);
								if (it->MonitorOutput)
									it->Instance->monitorOut(SocketStatusActivelyMonitored);

								nodes[i].H = it->Instance->getHandle();
								nodes[i].Input = it->MonitorInput;
								nodes[i].Output = it->MonitorOutput;
								nodes[i].CustomData = it->Instance;
								it++;
								i++;
							}
							else
							{
								it = this->monitored.erase(it);
								nodeCount--;
							}
						}
					}
					STDUTILS_RELEASE(this->listLock);

					uint32_t updates;
					this->semaphore.releaseLock();
					try
					{
						updates = WaitForSockets(nodes, nodeCount, timeoutMS);
						this->semaphore.acquireLock();
					}
					catch (...)
					{
						this->semaphore.acquireLock();
						delete[] nodes;
						throw;
					}

					for (uint32_t i = 1; i < nodeCount; i++)
					{
						Socket<SocketTypes::Any>* s = (Socket<SocketTypes::Any>*)nodes[i].CustomData;
						if (nodes[i].Input)
							s->monitorIn(SocketStatusMonitored);
						if (nodes[i].Output)
							s->monitorOut(SocketStatusMonitored);
					}

					if (this->updateSignals.Signaled) //Important: NOT nodes[0].InputUpdated, the signal may have been sent after WaitForIO returned
					{
						char type;
						this->updateSignals.Signalee.receive(&type, 1);

						switch (type)
						{
						case UpdateSignal:
							retry = true;
						case CancelSignal:
							//retry = false => loop exits => lock releases => waitForUpdates returns
							this->semaphore.signalAll(); //Can be done here, before Signaled is set back to false, because signalUpdate will try to reacquire the semaphore-lock before returning
							break;
						case DestructionSignal:
							throw Exception("SocketMonitor is destructing");
							break;
						}

						this->updateSignals.Signaled = false;
					}
					else
					{
						STDUTILS_LOCK(this->listLock)
						{
							uint32_t i = 0;
							while (i < nodeCount && updates > 0)
							{
								if (nodes[i].InputUpdated || nodes[i].OutputUpdated)
								{
									updates--;

									for (vector<MonitoredSocket>::iterator it = this->monitored.begin(); it != this->monitored.end(); it++)
									{
										if (it->Instance == nodes[i].CustomData)
										{
											rv.push_back(SocketUpdate());
											SocketUpdate& um(rv.back());

											um.Instance = it->Instance;
											um.InputUpdated = nodes[i].InputUpdated;
											um.OutputUpdated = nodes[i].OutputUpdated;
											um.CustomData = it->CustomData;

											break;
										}
									}
								}
								i++;
							}
						}
						STDUTILS_RELEASE(this->listLock);

#ifdef _DEBUG
						if (updates > 0)
						{
							delete[] nodes;
							throw Exception("DEBUG SocketMonitor::waitForIO: Not all updates where found in IOWaitNode array");
						}
#endif
					}

					delete[] nodes;
				} while (retry);

				this->waiting = false;
			}
			catch (...)
			{
				this->waiting = false;
				this->updateSignals.Signaled = false;
				this->semaphore.releaseLock();
				throw;
			}
			this->semaphore.releaseLock();


			return rv;
		}

		void SocketMonitor::cancelWait()
		{
			STDUTILS_SYNCHRONIZE(this->interfaceLock);

			this->signalUpdate(CancelSignal);
		}
	}
}
