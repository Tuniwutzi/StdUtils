#ifndef STDUTILS_HTTPSERVERCONTEXT_H
#define STDUTILS_HTTPSERVERCONTEXT_H

#include <StdUtils/Base.h>

#include <StdUtils/Networking/Http/HttpContext.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpServerContext : public HttpContext
			{
			public:
				explicit HttpServerContext(Socket<SocketTypes::Tcp>*, bool ownsSocket = true);
				explicit HttpServerContext(NetworkStream*, bool ownsStream = true);

			public:
				virtual ~HttpServerContext();

			public:
				bool requestReceived() const;
				void receiveRequest();

				IHttpBodyReader& getRequestBody();
				IHttpBodyWriter& sendResponse();
				bool responseSent() const;

				HttpRequest const& getRequest() const;
				HttpResponse const& getResponse() const;
				HttpResponse& getResponse();
				void setResponse(HttpResponse const& response);
			};
		}
	}
}

#endif
