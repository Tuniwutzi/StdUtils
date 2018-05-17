#ifndef STDUTILS_NETWORKING_HTTP_WEBSOCKETVERSIONS_H
#define STDUTILS_NETWORKING_HTTP_WEBSOCKETVERSIONS_H


#include <StdUtils/Base.h>


namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{		
			STDUTILS_START_ENUM(WebSocketVersions)
			{
				RFC6455
			}
			STDUTILS_END_ENUM(WebSocketVersions);
		}
	}
}

#endif