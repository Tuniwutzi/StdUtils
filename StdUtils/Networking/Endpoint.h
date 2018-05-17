#ifndef STDUTILS_ENDPOINT_H
#define STDUTILS_ENDPOINT_H

#include <StdUtils/Base.h>
#include <StdUtils/Networking/IPAddress.h>

namespace StdUtils
{
	namespace Networking
	{
		class Endpoint
		{
		private:
			IPAddress* address;
			unsigned short port;

		public:
			explicit Endpoint();
			explicit Endpoint(IPAddress const& address, unsigned short port = Endpoint::AnyPort);

			Endpoint(Endpoint const&);

			~Endpoint();

		public:
			Endpoint& operator=(Endpoint const&);

		public:
			bool isNull() const;

			IPAddress const& getAddress() const;
			unsigned short getPort() const;

		public:
			static const unsigned short AnyPort;
		};
	}
}

#endif