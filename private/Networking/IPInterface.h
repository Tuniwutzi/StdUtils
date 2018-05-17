#ifndef STDUTILS_PRIVATE_NETWORKING_IPINTERFACE_H
#define STDUTILS_PRIVATE_NETWORKING_IPINTERFACE_H


#include <StdUtils/Networking/Endpoint.h>
#include <private/Networking/Sockets.h>


namespace StdUtils
{
	namespace Networking
	{
		template<AddressTypes::Value ADDRESSTYPE>
		class IPInterface
		{
		private:
			IPInterface();

		public:
			static void connectToRemoteEndpoint(SocketHandle, Endpoint const&);
			static void bindToLocalEndpoint(SocketHandle, Endpoint const&);
			static SocketHandle acceptOnLocalEndpoint(SocketHandle, Endpoint const&, Endpoint* remoteEndpointBuffer);

			static Endpoint getLocalEndpoint(SocketHandle);
		};
	}
}

#endif