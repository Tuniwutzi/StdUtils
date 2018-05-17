#ifndef STDUTILS_HTTPCLIENTCONTEXT_H
#define STDUTILS_HTTPCLIENTCONTEXT_H

#include <StdUtils/Base.h>

#include <StdUtils/Networking/Http/HttpContext.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpClientContext : public HttpContext
			{
			public:
				explicit HttpClientContext(Socket<SocketTypes::Tcp>*, bool ownsSocket = true);
				explicit HttpClientContext(NetworkStream*, bool ownsStream = true);

			public:
				virtual ~HttpClientContext();

			public:
				IHttpBodyWriter& sendRequest();
				IHttpBodyReader& getResponseBody();

				HttpRequest& getRequest();
				void setRequest(HttpRequest const& request);

				bool responseReceived() const;
				HttpResponse const& receiveResponse();
				HttpResponse const& getResponse() const;
			};
		}
	}
}

#endif