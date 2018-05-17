#ifndef STDUTILS_WSENDPOINT_H
#define STDUTILS_WSENDPOINT_H


#include <StdUtils/Uri.h>
#include <StdUtils/Networking/Endpoint.h>
#include <StdUtils/Networking/SslStream.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class WSEndpoint
			{
			private:
				IPAddress* address;
				Nullable<String> host;
				uint16_t port;
				String ressource;

				Nullable<SslServerCertificate> sslCertificate;

			public:
				WSEndpoint();
				WSEndpoint(WSEndpoint const&);
				~WSEndpoint();

			public:
				WSEndpoint& operator=(WSEndpoint const&);

			private:
				void reset();
				void copy(WSEndpoint const&);

			public:
				//void setHost(Nullable<String> const&);
				//void setHost(Nullable<Endpoint> const&);
				//void setHost(Nullable<IPAddress> const&);
				//void setHost(Nullable<IPAddress> const&, Nullable<uint16_t> const&);
				void setIPEndpoint(Endpoint const&);
				void setIPEndpoint(IPAddress const&);
				void setIPEndpoint(IPAddress const&, uint16_t const&);
				void setRessource(String const&);
				void setHost(String const&);

				void setUri(Uri<UriFormats::WebSocket> const&);

				void setSslCertificate(Nullable<SslServerCertificate> const&);

				void setNull();


				bool isNull() const;
				
				Nullable<Endpoint> getIPEndpoint() const;
				Nullable<String> const& getHost() const;
				String const& getRessource() const;

				bool isSsl() const;
				Nullable<SslServerCertificate> const& getSslCertificate() const;
			};
		}
	}
}

#endif