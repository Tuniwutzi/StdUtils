#ifndef STDUTILS_PRIVATE_NETWORKING_SOCKETSTATUS_H
#define STDUTILS_PRIVATE_NETWORKING_SOCKETSTATUS_H


#include <StdUtils/Base.h>


namespace StdUtils
{
	namespace Private
	{
		namespace Networking
		{
			STDUTILS_START_ENUM(SocketStatus)
			{
				Closed			=	0x00,
				Connected		=	0x01,
				Disconnected	=	0x02,
				Bound			=	0x10,
				Listening		=	0x20
			}
			STDUTILS_END_ENUM(SocketStatus);
		}
	}
}

#endif