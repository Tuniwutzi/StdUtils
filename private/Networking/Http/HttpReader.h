#ifndef STDUTILS_HTTPREADER_H
#define STDUTILS_HTTPREADER_H

#include <StdUtils/Base.h>
#include <StdUtils/Networking/NetworkStream.h>

#include <StdUtils/Networking/Http/HttpMessage.h>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpReader
			{
			private:
				HttpReader();
			public:
				static void ReadHeaders(NetworkStream& s, HttpHeaders& buffer);
				static void ReadMessage(NetworkStream& s, HttpMessage& buffer);
			};
		}
	}
}

#endif