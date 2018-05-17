#ifndef STDUTILS_HTTPREQUEST_H
#define STDUTILS_HTTPREQUEST_H

#include <StdUtils/Base.h>
#include <StdUtils/Uri.h>
#include <StdUtils/Networking/Http/HttpMessage.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			STDUTILS_START_ENUM(KnownMethods)
			{
				GET,
				POST
			}
			STDUTILS_END_ENUM(KnownMethods);

			class HttpRequest : public HttpMessage
			{
			public:
				String method;
				Uri<UriFormats::Http> ressource;

			public:
				explicit HttpRequest();
				~HttpRequest();

			public:
				String const& getMethod() const;
				void setMethod(String const& method);

				Uri<UriFormats::Http> const& getRessource() const;
				Uri<UriFormats::Http>& getRessource();
				void setRessource(Uri<UriFormats::Http> const& ressource);
				void setRessource(String const& ressource);

				String getFirstLine() const;
				void setFirstLine(String const&);

				KnownMethods::Value getKnownMethod() const;
				void setKnownMethod(KnownMethods::Value method);

				String const& getHost() const;
				void setHost(String const&);
				void removeHost();

				Uri<UriFormats::Http> getRequestUri() const;
				void setRequestUri(Uri<UriFormats::Http> const&);

				String toString() const STDUTILS_OVERRIDE;
			};
		}
	}
}

#endif
