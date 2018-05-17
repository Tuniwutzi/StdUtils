#ifndef STDUTILS_PRIVATE_SOCKETS_H
#define STDUTILS_PRIVATE_SOCKETS_H


#include <StdUtils/Base.h>


#include <StdUtils/Networking/Socket.h>
#include <StdUtils/Exceptions.h>


#ifndef _WINDOWS

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //htonl
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKETERROR(s) (s < 0)
#define LASTSOCKETERROR errno
#define CREATESOCKET(domain, type, protocol) socket(domain, type, protocol)
#define CLOSESOCKET(s) close(s)

#else

#include <WinSock2.h>

typedef int socklen_t;
#define SOCKETERROR(s) (s == SOCKET_ERROR)
#define LASTSOCKETERROR WSAGetLastError()
#define CREATESOCKET(domain, type, protocol) (StdUtils::Private::Networking::InitializeWSA(), socket(domain, type, protocol))
#define CLOSESOCKET(s) closesocket(s)

#endif

namespace StdUtils
{
	namespace Private
	{
		namespace Networking
		{
#ifdef _WINDOWS
			inline void InitializeWSA()
			{
				static bool initialized(false);
				if (!initialized)
				{
					WSADATA w;
					int errcode;
					if ((errcode = WSAStartup(MAKEWORD(2,2), &w)) != 0)
						throw OSApiException("WinSock2 konnte nicht gestartet werden", errcode);
					else
						initialized = true;
				}
			}
#endif

#ifdef _WINDOWS
			inline void MakeSocketNonBlocking(StdUtils::Networking::SocketHandle handle)
			{
				unsigned long val = 1;
				int rv = ioctlsocket(handle, FIONBIO, &val);
				if (SOCKETERROR(rv))
					throw OSApiException("Error putting socket in non-blocking mode", LASTSOCKETERROR);
			}
#else
			inline void MakeSocketNonBlocking(StdUtils::Networking::SocketHandle handle)
			{
				int flags = fcntl(handle, F_GETFL, 0); //Gesetzte flags lesen
				if (flags == -1)
					throw OSApiException("Error reading existing flags of tcp socket", errno);
				flags = flags | O_NONBLOCK; //nonblock flag setzen
				if (fcntl(handle, F_SETFL, flags) == -1)
					throw OSApiException("Error putting socket in non-blocking mode", errno);
			}
#endif
		}
	}
}

#endif
