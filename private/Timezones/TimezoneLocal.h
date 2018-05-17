#ifndef STDUTILS_TIMEZONELOCAL_H
#define STDUTILS_TIMEZONELOCAL_H

#include <StdUtils/DateTime.h>

namespace StdUtils
{
	class TimezoneLocal : public Timezone
	{
	public:
		virtual ~TimezoneLocal() {}
	public:
		DateTime now() const STDUTILS_OVERRIDE;
		String const& toString() const STDUTILS_OVERRIDE;
	};
}

#endif
