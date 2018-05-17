#include <StdUtils/Timezone.h>


#include <private/Timezones/TimezoneLocal.h>
#include <private/Timezones/TimezoneUtc.h>


namespace StdUtils
{
	bool Timezone::operator==(Timezone const& other) const
	{
		return this == &other;
	}
	bool Timezone::operator!=(Timezone const& other) const
	{
		return !this->operator==(other);
	}
	JSTimestamp Timezone::operator-(Timezone const& other) const
	{
		return this->now().getJSTimestamp() - other.now().getJSTimestamp();
	}

	Date Timezone::getCurrentDate() const
	{
		return this->now().getDate();
	}
	Time Timezone::getCurrentTime() const
	{
		return this->now().getTime();
	}

	DateTime Timezone::convert(DateTime const& other) const
	{
		if (other.getTimezone() == *this)
			return other;
		else
		{
			JSTimestamp otherTS(other.getJSTimestamp());
			JSTimestamp difference(*this - other.getTimezone());

			return DateTime::FromJSTimestamp(otherTS + difference, *this);
		}
	}

	Timezone const& Timezone::Local()
	{
		static TimezoneLocal tz;
		return tz;
	}
	Timezone const& Timezone::Utc()
	{
		static TimezoneUtc tz;
		return tz;
	}
}
