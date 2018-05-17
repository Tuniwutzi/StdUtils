#ifndef STDUTILS_NETWORKING_SOCKETMONITOR_H
#define STDUTILS_NETWORKING_SOCKETMONITOR_H


#include <vector>
#include <map>

#include <StdUtils/Networking/Socket.h>
#include <StdUtils/Threading/Mutex.h>
#include <StdUtils/Threading/ManagedSemaphore.h>
#include <StdUtils/Networking/TcpSocket.h>


namespace StdUtils
{
	namespace Networking
	{
		struct SocketUpdate
		{
			Socket<SocketTypes::Any>* Instance;

			bool InputUpdated;
			bool OutputUpdated;

			void* CustomData;
		};
		typedef std::vector<SocketUpdate> SocketUpdates;

		//TODO?: Mechanismus, um beim Schliessen einer ueberwachten Socket Exceptions zu vermeiden und stattdessen das Handle im SocketMonitor zum Schliessen zu queuen. Wichtig, um das Problem zu vermeiden, dass durch automatischen Aufruf von close in der Socket beim Aktualisieren des Zustands einer geschlossenen Socket entsteht, wo man bspw. refreshState an einer Socket aufruft die gerade in einem anderen Thread ueberwacht wird und eine Exception bekommt, laut der man angeblich versucht hat, die Socket zu schliessen waehrend sie noch ueberwacht wurde. Vorlaeufige Alternative zu komplexen Mechanismus; Andere Exception, die IMMER kommt, wenn man nicht-const-Sachen (Sachen, die Dinge mit dem Handle machen) mit einer Socket zu machen sucht, die gerade aktiv ueberwacht wird (WICHTIG: Ausser beim senden! .send muss machbar sein, waehrend ueberwacht wird!)
		//TODO: Interface fuer performantes Hinzufuegen mehrerer Handles, ohne jedes Mal einen evtl. laufenden waitForUpdates-Call zu alerten.
		//Vorschlaege: 1.: set(map<IMonitorable*, void*>, bool, bool) 2.: queueSet(IMonitorable*, bool, bool void*); executeQueuedSets(); (seperater Vektor: vector<Monitored> queuedSets;)
		class SocketMonitor
		{
			STDUTILS_NOCOPY(SocketMonitor);

		private:
			struct MonitoredSocket
			{
				Socket<SocketTypes::Any>* Instance;
				void* CustomData;

				bool MonitorInput;
				bool MonitorOutput;
			};

		public:
			typedef std::vector<MonitoredSocket>::size_type size_type;

		private:
			StdUtils::Threading::Mutex interfaceLock;
			StdUtils::Threading::Mutex listLock;
			StdUtils::Threading::ManagedSemaphore semaphore;

			std::vector<MonitoredSocket> monitored;

			bool waiting;

			struct
			{
				StdUtils::Networking::TcpSocket Signaler;
				StdUtils::Networking::TcpSocket Signalee;
				bool Signaled;
			} updateSignals;

		public:
			SocketMonitor();
			~SocketMonitor();

		private:
			void initializeSignals();
			void signalUpdate(char);

		public:
			/*
			monitorInput == false && monitorOutput == false => remove IMonitorable from SocketMonitor
			*/
			void set(Socket<SocketTypes::Any>*, bool monitorInput, bool monitorOutput, void* customData = NULL);
			void remove(Socket<SocketTypes::Any>*);

			size_type getMonitoredCount() const;

			SocketUpdates waitForUpdate(uint32_t timeoutMS = 0);

			//If another thread is currently paused in waitForUpdate(), it will return
			void cancelWait();
		};
	}
}

#endif
