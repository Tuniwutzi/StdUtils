#ifndef STDUTILS_IHTTPBODYWRITER_H
#define STDUTILS_IHTTPBODYWRITER_H

#include <StdUtils/Base.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class IHttpBodyWriter
			{
			public:
				virtual bool knowsWriteableBytes() const = 0;
				virtual uint64_t getWriteableBytes() const = 0;

				virtual void write(String const& str) = 0;
				virtual void write(char const* buf, uint64_t length) = 0;
				
				virtual bool isFinished() const = 0;
			};
		}
	}
}

#endif