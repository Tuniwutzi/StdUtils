#ifndef STDUTILS_IHTTPBODYREADER_H
#define STDUTILS_IHTTPBODYREADER_H

#include <StdUtils/Base.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class IHttpBodyReader
			{
			public:
				virtual bool knowsReadableBytes() const = 0;
				virtual uint64_t getReadableBytes() const = 0;

				virtual String read() = 0;
				virtual uint64_t read(char* buf, uint64_t bufferSize) = 0;
				
				virtual bool isFinished() const = 0;
			};
		}
	}
}

#endif