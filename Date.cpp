#include <StdUtils/DateTime.h>


#include <ctime>
#include <time.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>


namespace StdUtils
{
	Date::Date(Timezone const& tz)
		: timezone(&tz),
		year(1970),
		month(1),
		day(1)
	{
	}

	bool Date::operator ==(Date const& d) const
	{
		return this->timezone == d.timezone && this->year == d.year && this->month == d.month && this->day == d.day;
	}
	bool Date::operator !=(Date const& d) const
	{
		return !this->operator ==(d);
	}

	bool Date::operator<(Date const& other) const
	{
		return this->getJSTimestamp() < other.getJSTimestamp();
		//if (this->year <= other.year)
		//{
		//	if (this->year == other.year)
		//	{
		//		if (this->month <= other.month)
		//		{
		//			if (this->month == other.month)
		//				return this->day < other.day;
		//			else
		//				return true;
		//		}
		//		else
		//			return false;
		//	}
		//	else
		//		return true;
		//}
		//else
		//	return false;
	}
	bool Date::operator<=(Date const& other) const
	{
		return (*this) < other || (*this) == other;
	}

	Date::Day Date::getDay() const
	{
		return this->day;
	}

	Date::Month Date::getMonth() const
	{
		return this->month;
	}

	Date::Year Date::getYear() const
	{
		return this->year;
	}

	void Date::setDay(Day day)
	{
		if (day <= 31 && day > 0)
			this->day = day;
		else
			throw Exception(String("Invalid Day value: ") + StringHelpers::ToString(day));
	}
	void Date::setMonth(Month month)
	{
		if (month < 13 && month > 0)
			this->month = month;
		else
			throw Exception(String("Invalid Month value: ") + StringHelpers::ToString(month));
	}
	void Date::setYear(Year year)
	{
		this->year = year;
	}

	Timezone const& Date::getTimezone() const
	{
		return *this->timezone;
	}

	JSTimestamp Date::getJSTimestamp() const
	{
		tm time;
		time.tm_hour = 0;
		time.tm_isdst = -1;
		time.tm_mday = this->day;
		time.tm_min = 0;
		time.tm_mon = this->month - 1;
		time.tm_sec = 0;
		time.tm_year = (this->year - 1900);

#ifdef _WINDOWS
		__time64_t t = _mktime64(&time);
#else
		int64_t t = mktime(&time); //time_t is 32 Bit -> overflows
#endif

		return t * 1000;
	}

	String Date::format(DateFormat const& format) const
	{
		return format.Format(*this);
	}
	String Date::toString() const
	{
		return StringHelpers::ToString(this->day) + "." + StringHelpers::ToString(this->month) + "." + StringHelpers::ToString(this->year);
	}

	Date Date::Parse(String const& raw, DateFormat const& format, Timezone const& tz)
	{
		Date buffer(tz);
		format.Parse(buffer, raw);

		return buffer;
	}

	Date Date::Today(Timezone const& tz)
	{
		return tz.getCurrentDate();
	}

	Date Date::FromJSTimestamp(JSTimestamp timestamp, Timezone const& tz)
	{
		static uint16_t hourS = 60 * 60;

		Date buffer(tz);

		time_t t = (timestamp / 1000);// + hourS;
		tm time;

#ifdef _WINDOWS
		errno_t err = gmtime_s(&time, &t);
		if (err)
			throw OSApiException("Error determining time; gmtime_s", err);
#else //POSIX		tm* rv = gmtime_r(&t, &time);
		if (rv == NULL)
			throw OSApiException("Error determining time; gmtime_r", errno);
#endif
		buffer.day = time.tm_mday;
		buffer.month = time.tm_mon + 1;
		buffer.year = (time.tm_year + 1900);

		return buffer;
	}
}