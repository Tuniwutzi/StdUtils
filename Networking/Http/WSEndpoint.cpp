#include <StdUtils/Networking/Http/WSEndpoint.h>


#include <StdUtils/Networking/DomainNameSystem.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			WSEndpoint::WSEndpoint()
				:address(NULL),
				port(80)
			{
			}
			WSEndpoint::WSEndpoint(WSEndpoint const& o)
			{
				this->copy(o);
			}
			WSEndpoint::~WSEndpoint()
			{
				this->reset();
			}

			WSEndpoint& WSEndpoint::operator=(WSEndpoint const& o)
			{
				this->reset();
				this->copy(o);

				return *this;
			}

			void WSEndpoint::reset()
			{
				delete this->address;
				this->address = NULL;

				this->host.setNull();
				this->port = 80;
				this->ressource.clear();

				this->sslCertificate.setNull();
			}

			void WSEndpoint::copy(WSEndpoint const& o)
			{
				if (o.address)
					this->address = o.address->copy();
				else
					this->address = NULL;

				this->host = o.host;
				this->port = o.port;
				this->ressource = o.ressource;

				this->sslCertificate = o.sslCertificate;
			}

			void WSEndpoint::setIPEndpoint(Endpoint const& ep)
			{
				this->address = ep.getAddress().copy();
				this->port = ep.getPort();
			}
			void WSEndpoint::setIPEndpoint(IPAddress const& address)
			{
				this->setIPEndpoint(Endpoint(address, 80));
			}
			void WSEndpoint::setIPEndpoint(IPAddress const& address, uint16_t const& port)
			{
				this->setIPEndpoint(Endpoint(address, port));
			}
			void WSEndpoint::setRessource(String const& ressource)
			{
				this->ressource = ressource;
			}
			void WSEndpoint::setHost(String const& host)
			{
				this->host = host;
			}

			void WSEndpoint::setUri(Uri<UriFormats::WebSocket> const& uri)
			{
				this->reset();

				if (!uri.isRelative())
				{
					this->host = uri.getHost();
					this->port = uri.getPort().hasValue() ? uri.getPort().getValue() : 80;

					if (this->host.hasValue())
					{
						try
						{
							this->address = new IPv4Address(this->host.getValue());
						}
						catch (...)
						{
						}

						//IPv6:
						/*
						if (!this->address)
						{
							... potentiell checken, ob der host mit [] eingeklammert ist
						}
						*/

						if (!this->address)
						{
							DNSInformation dni(DomainNameSystem::Resolve(this->host.getValue()));

							this->address = dni.getFirstAddress().copy();
						}
					}
				}

				this->ressource = uri.getPath();
			}

			void WSEndpoint::setSslCertificate(StdUtils::Nullable<SslServerCertificate> const& cert)
			{
				this->sslCertificate = cert;
			}

			void WSEndpoint::setNull()
			{
				this->reset();
			}


			bool WSEndpoint::isNull() const
			{
				return this->address == NULL;
			}

			Nullable<Endpoint> WSEndpoint::getIPEndpoint() const
			{
				if (this->isNull())
					return Nullable<Endpoint>();
				else
					return Endpoint(*address, this->port);
			}
			Nullable<String> const& WSEndpoint::getHost() const
			{
				return this->host;
			}
			String const& WSEndpoint::getRessource() const
			{
				return this->ressource;
			}

			bool WSEndpoint::isSsl() const
			{
				return this->sslCertificate.hasValue();
			}
			Nullable<SslServerCertificate> const& WSEndpoint::getSslCertificate() const
			{
				return this->sslCertificate;
			}
		}
	}
}
