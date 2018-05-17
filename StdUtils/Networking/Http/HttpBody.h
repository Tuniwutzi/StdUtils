#ifndef STDUTILS_HTTPBODY_H
#define STDUTILS_HTTPBODY_H

#include <StdUtils/Base.h>

#include <StdUtils/Networking/SslStream.h>

#include <StdUtils/Networking/Http/IHttpBodyReader.h>
#include <StdUtils/Networking/Http/IHttpBodyWriter.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpBody : public IHttpBodyReader, public IHttpBodyWriter
			{
			private:
				bool receiving;
				bool closed;

			protected:
				NetworkStream& socket;

			protected:
				explicit HttpBody(NetworkStream& socket, bool receiving);

			public:
				virtual ~HttpBody() {}

			protected:
				void assertRead() const;
				void assertWrite() const;

			public:
				virtual void close();

				bool isOpen() const;

				bool isWritingBody() const;
				bool isReadingBody() const;
			};
		}
	}
}

#endif
