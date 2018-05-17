#ifndef STDUTILS_NETWORKING_HTTP_WSMESSAGETYPES_H
#define STDUTILS_NETWORKING_HTTP_WSMESSAGETYPES_H


#include <StdUtils/Base.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			STDUTILS_START_ENUM(WSMessageTypes)
			{
				Binary,
				Text,

				Unknown
			}
			STDUTILS_END_ENUM(WSMessageTypes);
		}
	}
}

#endif