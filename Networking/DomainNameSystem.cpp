#include <StdUtils/Networking/DomainNameSystem.h>


#include <StdUtils/Exceptions.h>

#ifdef _WINDOWS
#include <Ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> //htonl
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif


using namespace std;


namespace StdUtils
{
	namespace Networking
	{
		DNSInformation DomainNameSystem::Resolve(String const& hostName)
		{
#ifdef _WINDOWS
			WSADATA w;
			if (WSAStartup(MAKEWORD(2,2), &w) != 0)
				throw OSApiException("Winsock2 konnte nicht gestartet werden", WSAGetLastError());
#endif
			vector<IPAddress const*> allAddresses;
			vector<IPv4Address> ipv4Addresses;

			addrinfo* result = NULL;
			int res = getaddrinfo(hostName.data(), NULL, NULL, &result);

			if (res != 0)
				throw OSApiException("Error resolving host name", res);

			for (addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next)
			{
				switch (ptr->ai_family)
				{
				case AF_INET:
					{
						sockaddr_in* inaddr((sockaddr_in*)ptr->ai_addr);
#ifdef _WINDOWS
						ipv4Addresses.push_back(IPv4Address(inaddr->sin_addr.S_un.S_addr));
#else
						ipv4Addresses.push_back(IPv4Address(inaddr->sin_addr.s_addr));
#endif
					//Geht nicht hier, da der Vektor bei nachfolgenden Vergroesserungen sonst die Speicherstelle verschiebt
						//allAddresses.push_back(&*ipv4Addresses.rbegin());
					}
					break;
				case AF_INET6:
					//TODO: Implementieren
					break;
				default:
					throw Exception("Unexpected address family");
					break;
				}
			}

			freeaddrinfo(result);

			for (vector<IPv4Address>::const_iterator it = ipv4Addresses.begin(); it != ipv4Addresses.end(); it++)
				allAddresses.push_back(&*it);

			//hostent* host = gethostname(hostName.data());
			//if (!host)
			//	throw OSApiException(String("Error resolving host '") + hostName + "'", WSAGetLastError());

			//in_addr* addresses = (in_addr*)host->h_addr_list;
			//while (addresses)
			//{
			//	ipv4Addresses.push_back(IPv4Address(addresses->S_un.S_addr));
			//	allAddresses.push_back(&*ipv4Addresses.rbegin());

			//	addresses++;
			//}

#ifdef _WINDOWS
			WSACleanup();
#endif

			return DNSInformation(hostName, allAddresses);
		}
	}
}