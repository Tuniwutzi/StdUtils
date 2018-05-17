//Prevent miniwindef.h from defining min/max (For WaitForSockets.h)
#define NOMINMAX

#include <StdUtils/Networking/Socket.h>


#include <StdUtils/Exceptions.h>
#include <StdUtils/Threading/Mutex.h>

#include <private/Networking/WaitForSockets.h>


using namespace StdUtils::Threading;
using namespace StdUtils::Private::Networking;


namespace StdUtils
{
	namespace Networking
	{
		//Necessary, for example:
		//Thread ta using SocketMonitor ma, Thread tb using SocketMonitor mb, Socket s. in ta: ma.set(s, true, false); in tb: mb.set(s, true, false);
		//race in setMonitoringState: if (!this->inputMonitored) this->inputMonitored = true; else throw ...;
		//TODO: Elegantere Loesung; Muss nicht global fuer alle Socket-Instanzen sein, ist nur hier, damit es nicht im Header sein muss. Waere schon moeglich global fuer alle SocketMonitor-Instanzen...
		static Mutex SocketMonitoringLock;

		Socket<SocketTypes::Any>::Socket()
			:socketHandle(0),
			inputMonitored(SocketStatusNone),
			outputMonitored(SocketStatusNone),
			handle(socketHandle)
		{
		}

		Socket<SocketTypes::Any>::~Socket()
		{
		}

		void Socket<SocketTypes::Any>::monitorIn(int status)
		{
			STDUTILS_LOCK(SocketMonitoringLock)
			{
				if (status == SocketStatusNone && this->inputMonitored == SocketStatusMonitored)
				{
					this->inputMonitored = status;
				}
				else if (status == SocketStatusMonitored)
				{
					if (this->inputMonitored == SocketStatusMonitored)
						throw Exception("The input of this Socket is already being monitored");
					else
						this->inputMonitored = status;
				}
				else if (status == SocketStatusActivelyMonitored && this->inputMonitored == SocketStatusMonitored)
					this->inputMonitored = status;
				else
					throw Exception("Invalid input monitoring status set to Socket");
			}
			STDUTILS_RELEASE(SocketMonitoringLock);
		}
		void Socket<SocketTypes::Any>::monitorOut(int status)
		{
			STDUTILS_LOCK(SocketMonitoringLock)
			{
				if (status == SocketStatusNone && this->outputMonitored == SocketStatusMonitored)
				{
					this->outputMonitored = status;
				}
				else if (status == SocketStatusMonitored)
				{
					if (this->outputMonitored == SocketStatusMonitored)
						throw Exception("The output of this Socket is already being monitored");
					else
						this->outputMonitored = status;
				}
				else if (status == SocketStatusActivelyMonitored && this->outputMonitored == SocketStatusMonitored)
					this->outputMonitored = status;
				else
					throw Exception("Invalid output monitoring status set to Socket");
			}
			STDUTILS_RELEASE(SocketMonitoringLock);
		}

		SocketHandle Socket<SocketTypes::Any>::resetSocketHandle()
		{
			STDUTILS_LOCK(SocketMonitoringLock)
			{
				if (this->inputMonitored != SocketStatusActivelyMonitored && this->outputMonitored != SocketStatusActivelyMonitored)
				{
					SocketHandle h = this->handle;
					this->socketHandle = 0;
					STDUTILS_RELEASE_AND_RETURN(SocketMonitoringLock, h);
				}
				else
					throw Exception("Handle cannot be closed, Socket is still monitored by a SocketMonitor");
			}
			STDUTILS_RELEASE(SocketMonitoringLock);
		}

		void Socket<SocketTypes::Any>::createSocketHandle(Socket<SocketTypes::Any>::CreateFunction cf)
		{
			STDUTILS_LOCK(SocketMonitoringLock)
			{
				if (this->handle != 0)
					throw Exception("Cannot create new socket handle while the old handle still exists");
				if (this->inputMonitored != SocketStatusActivelyMonitored && this->outputMonitored != SocketStatusActivelyMonitored)
					this->socketHandle = cf();
				else
					throw Exception("Handle cannot be changed, Socket is still monitored by a SocketMonitor");
			}
			STDUTILS_RELEASE(SocketMonitoringLock);
		}

		void Socket<SocketTypes::Any>::setSocketHandle(SocketHandle h)
		{
			STDUTILS_LOCK(SocketMonitoringLock)
			{
				if (this->handle != 0)
					throw Exception("Cannot set new socket handle while the old handle still exists");
				if (this->inputMonitored != SocketStatusActivelyMonitored && this->outputMonitored != SocketStatusActivelyMonitored)
					this->socketHandle = h;
				else
					throw Exception("Handle cannot be changed, Socket is still monitored by a SocketMonitor");
			}
			STDUTILS_RELEASE(SocketMonitoringLock);
		}
		SocketHandle Socket<SocketTypes::Any>::getHandle() const
		{
			return this->handle;
		}
	}
}