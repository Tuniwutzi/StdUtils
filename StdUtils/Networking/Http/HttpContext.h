#ifndef STDUTILS_HTTPCONTEXT_H
#define STDUTILS_HTTPCONTEXT_H

#include <StdUtils/Base.h>

#include <StdUtils/Networking/Http/HttpRequest.h>
#include <StdUtils/Networking/Http/HttpResponse.h>
#include <StdUtils/Networking/Http/HttpBody.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			STDUTILS_START_ENUM(HttpContextStates)
			{
				Waiting,
				Receiving,
				Sending,
				Closed,
				Error
			}
			STDUTILS_END_ENUM(HttpContextStates);
			
			class HttpContext
			{
				STDUTILS_NOCOPY(HttpContext);

			protected:
				NetworkStream* stream;
				SocketStream* sstream;

				bool doesOwnStream;

				HttpRequest request;
				HttpResponse response;

				HttpBody* body;

				bool errorState;

			protected:
				HttpContext(Socket<SocketTypes::Tcp>*, bool ownsSocket);
				HttpContext(NetworkStream*, bool ownsStream = true);

			public:
				virtual ~HttpContext();

			protected:
				void assertWaiting() const;
				void assertReceiving() const;
				void assertSending() const;

				void setErrorState();

				HttpBody* createBody(HttpMessage const& message, bool receiving) const;

			public:
				HttpContextStates::Value getState() const;

				NetworkStream* getStream() const;

				void ownStream();
				void disownStream();
				bool ownsStream() const;

				void close();
			};
		}
	}
}

#endif