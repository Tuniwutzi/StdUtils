#ifndef STDUTILS_SERVICESIGNALS_H
#define STDUTILS_SERVICESIGNALS_H

#include <StdUtils/Base.h>

namespace StdUtils
{
	namespace Application
	{
		STDUTILS_START_ENUM(ServiceSignals)
		{
			Start,
				Stop
		}
		STDUTILS_END_ENUM(ServiceSignals);
	}
}

#endif