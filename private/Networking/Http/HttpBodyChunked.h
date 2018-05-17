#ifndef STDUTILS_HTTPBODYCHUNKED_H
#define STDUTILS_HTTPBODYCHUNKED_H

#include <StdUtils/Networking/Http/HttpBody.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpBodyChunked : public HttpBody
			{
			private:
				unsigned int transferred;
				bool knowsLength;

			public:
				explicit HttpBodyChunked(NetworkStream& s, bool receiving);
				~HttpBodyChunked();

			public:
				bool knowsWriteableBytes() const STDUTILS_OVERRIDE;
				uint64_t getWriteableBytes() const STDUTILS_OVERRIDE;
				void write(String const& str) STDUTILS_OVERRIDE;
				void write(char const* buf, uint64_t length) STDUTILS_OVERRIDE;

				bool knowsReadableBytes() const STDUTILS_OVERRIDE;
				uint64_t getReadableBytes() const STDUTILS_OVERRIDE;
				String read() STDUTILS_OVERRIDE;
				uint64_t read(char* buf, uint64_t bufferSize) STDUTILS_OVERRIDE;
				
				bool isFinished() const STDUTILS_OVERRIDE;

				void close() STDUTILS_OVERRIDE;
			};
		}
	}
}

#endif