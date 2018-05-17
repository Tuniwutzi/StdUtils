#ifndef STDUTILS_SOCKET_H
#define STDUTILS_SOCKET_H

//Tell stdint.h to include x_MAX definitions
#define __STDC_LIMIT_MACROS

#include <StdUtils/Base.h>
#include <StdUtils/Networking/Endpoint.h>


#ifdef _WINDOWS
#include <WinSock2.h>
#endif


namespace StdUtils
{
	namespace Networking
	{
		STDUTILS_START_ENUM(SocketTypes)
		{
			Any,
				Tcp
		}
		STDUTILS_END_ENUM(SocketTypes);

		template<SocketTypes::Value PROTOCOL>
		class Socket;

		class SocketMonitor;

#ifdef _WINDOWS
		typedef SOCKET SocketHandle;
#else
		typedef uint32_t SocketHandle;
#endif

		//Note: It is NOT supported to use "state-changing" methods on a socket, that is currently being monitored in a different thread, without locks. As in: The SocketMonitor::waitForUpdates-Method may NOT be active, while state-changing methods are called. state-changing meaning: Opening/closing/changing the handle. send/receive or status queries are allowed.
		template<>
		class Socket<SocketTypes::Any>
		{
			friend class SocketMonitor;

		private:
			typedef SocketHandle (*CreateFunction)();

		private:
			SocketHandle socketHandle;

			int inputMonitored;
			int outputMonitored;

		protected:
			SocketHandle const& handle;

		public:
			Socket();
			virtual ~Socket();

		private:
			void monitorIn(int);
			void monitorOut(int);

		protected:
			//Throws if input or output is being monitored
			//The SocketHandle returned is guaranteed to be safely closeable
			SocketHandle resetSocketHandle();
			//Throws, if input or output is being monitored
			//Otherwise uses CreateFunction to create and store a handle
			void createSocketHandle(CreateFunction);
			//Throws, if input or output is being monitored
			//Otherwise stores the given handle
			void setSocketHandle(SocketHandle);

		public:
			SocketHandle getHandle() const;
		};
	}
}

#endif
