#include <private/Timezones/TimezoneLocal.h>


#include <StdUtils/Exceptions.h>

#include <ctime>
#include <time.h>

namespace StdUtils
{
	StdUtils::DateTime TimezoneLocal::now() const
	{
		DateTime dt(*this);

		time_t ts = time(0);   // get time now
		tm now;

#ifdef _WINDOWS
		errno_t err = localtime_s(&now, &ts);
		if (err)
			throw OSApiException("Could not determine current time", err);
#else
		if (!localtime_r(&ts, &now))
			throw OSApiException("Could not determine current time", errno);
#endif

		Time& t(dt.getTime());
		t.setHour(now.tm_hour);
		t.setMinute(now.tm_min);
		t.setSecond(now.tm_sec);
		t.setMillisecond(0);

		Date& d(dt.getDate());
		d.setDay(now.tm_mday);
		d.setMonth(now.tm_mon + 1);
		d.setYear(now.tm_year + 1900);

		return dt;
	}
	String const& TimezoneLocal::toString() const
	{
		static String str("Local");
		return str;
	}
}
