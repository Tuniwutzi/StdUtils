#include <private/Networking/IPInterface.h>


#include <cassert>

#include <StdUtils/Exceptions.h>


namespace StdUtils
{
	namespace Networking
	{
		static void fillSockaddr_in(sockaddr_in& addr_in, Endpoint const& ep)
		{
			addr_in.sin_family = AF_INET;
			addr_in.sin_port = htons(ep.getPort());
			IPv4Address const& ipAddress((IPv4Address const&)ep.getAddress());
#ifdef _WINDOWS
			addr_in.sin_addr.S_un.S_addr = ipAddress.asBigEndian();
#else
			addr_in.sin_addr.s_addr = ipAddress.asBigEndian();
#endif
		}
		static Endpoint extractEndpoint(sockaddr_in const& addr_in)
		{
#ifdef _WINDOWS
				IPv4Address address(addr_in.sin_addr.S_un.S_addr);
#else
				IPv4Address address(addr_in.sin_addr.s_addr);
#endif
				unsigned short port = ntohs(addr_in.sin_port);

				return Endpoint(address, port);
		}

		template<>
		void IPInterface<AddressTypes::IPv4>::connectToRemoteEndpoint(SocketHandle h, Endpoint const& remoteEndpoint)
		{
			assert(remoteEndpoint.getAddress().getType() == AddressTypes::IPv4);

			sockaddr_in addr_in;
			sockaddr* address = (sockaddr*) &addr_in;
			socklen_t size(sizeof(sockaddr_in));

			fillSockaddr_in(addr_in, remoteEndpoint);

			if (SOCKETERROR(connect(h, address, size)))
				throw OSApiException("Connection failed", LASTSOCKETERROR);
		}

		template<>
		void IPInterface<AddressTypes::IPv4>::bindToLocalEndpoint(SocketHandle h, Endpoint const& localEndpoint)
		{
			assert(localEndpoint.getAddress().getType() == AddressTypes::IPv4);

			sockaddr_in addr_in;
			sockaddr* address = (sockaddr*) &addr_in;
			socklen_t size(sizeof(sockaddr_in));

			fillSockaddr_in(addr_in, localEndpoint);

			if (SOCKETERROR(bind(h, address, size)))
				throw OSApiException("Bind failed", LASTSOCKETERROR);
		}

		template<>
		SocketHandle IPInterface<AddressTypes::IPv4>::acceptOnLocalEndpoint(SocketHandle h, Endpoint const& localEndpoint, Endpoint* remoteEndpointBuffer)
		{
			assert(localEndpoint.getAddress().getType() == AddressTypes::IPv4);

			sockaddr_in addr_in;
			sockaddr* address = (sockaddr*) &addr_in;
			socklen_t size(sizeof(sockaddr_in));

			SocketHandle newSocket = accept(h, address, &size);
			if (!SOCKETERROR(newSocket))
			{
				if (remoteEndpointBuffer)
					*remoteEndpointBuffer = extractEndpoint(addr_in);
				return newSocket;
			}
			else
				throw OSApiException("Could not accept connection", LASTSOCKETERROR);
		}

		template<>
		Endpoint IPInterface<AddressTypes::IPv4>::getLocalEndpoint(SocketHandle h)
		{
			sockaddr_in addr_in;
			sockaddr* address = (sockaddr*) &addr_in;
			socklen_t size(sizeof(sockaddr_in));

			if (!SOCKETERROR(getsockname(h, address, &size)))
				return extractEndpoint(addr_in);
			else
				throw OSApiException("Could not retrieve local endpoint", LASTSOCKETERROR);
		}
	}
}