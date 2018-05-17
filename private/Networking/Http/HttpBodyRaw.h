#ifndef STDUTILS_HTTPBODYRAW_H
#define STDUTILS_HTTPBODYRAW_H

#include <StdUtils/Networking/Http/HttpBody.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpBodyRaw : public HttpBody
			{
			private:
				uint64_t transferred;
				uint64_t contentLength;
				bool knowsLength;

				TcpStream* tcpStream;

			public:
				explicit HttpBodyRaw(NetworkStream& s, bool receiving, uint64_t contentLength);
				explicit HttpBodyRaw(NetworkStream& s, bool receiving);
				~HttpBodyRaw();

			public:
				bool knowsWriteableBytes() const STDUTILS_OVERRIDE;
				uint64_t getWriteableBytes() const STDUTILS_OVERRIDE;
				void write(String const& str) STDUTILS_OVERRIDE;
				void write(char const* buf, uint64_t length) STDUTILS_OVERRIDE;

				bool knowsReadableBytes() const STDUTILS_OVERRIDE;
				uint64_t getReadableBytes() const STDUTILS_OVERRIDE;
				bool isFinished() const STDUTILS_OVERRIDE;
				String read() STDUTILS_OVERRIDE;
				uint64_t read(char* buf, uint64_t bufferSize) STDUTILS_OVERRIDE;
			};
		}
	}
}

#endif