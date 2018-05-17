#include <StdUtils/DateTime.h>


#include <ctime>
#include <time.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>


namespace StdUtils
{
	Time::Time(Timezone const& tz)
		: timezone(&tz),
		millisecond(0),
		second(0),
		minute(0),
		hour(0)
	{
	}

	bool Time::operator ==(Time const& t) const
	{
		return this->timezone == t.timezone && this->hour == t.hour && this->minute == t.minute && this->second == t.second && this->millisecond == t.millisecond;
	}
	bool Time::operator !=(Time const& t) const
	{
		return !this->operator ==(t);
	}

	bool Time::operator<(Time const& other) const
	{
		return this->getJSTimestamp() < other.getJSTimestamp();
	}

	Time::Hours Time::getHour() const
	{
		return this->hour;
	}
	Time::Minutes Time::getMinute() const
	{
		return this->minute;
	}
	Time::Seconds Time::getSecond() const
	{
		return this->second;
	}
	Time::Milliseconds Time::getMillisecond() const
	{
		return this->millisecond;
	}

	Timezone const& Time::getTimezone() const
	{
		return *this->timezone;
	}

	//TODO: Uebergabewerte pruefen
	//TODO: Add-Methoden, die Berechnungen uebernehmen (AddSeconds(120) erhoeht die Minutenzahl um 2)
	void Time::setHour(Hours hour)
	{
		this->hour = hour;
	}
	void Time::setMinute(Minutes minute)
	{
		this->minute = minute;
	}
	void Time::setSecond(Seconds second)
	{
		this->second = second;
	}
	void Time::setMillisecond(Milliseconds milliseconds)
	{
		this->millisecond = millisecond;
	}

	JSTimestamp Time::getJSTimestamp() const
	{
		int minutes = this->minute + (this->hour * 60);
		int seconds = this->second + (minutes * 60);
		int milliseconds = this->millisecond + (seconds * 1000);

		return (milliseconds);// - (60 * 60 * 1000)); //JS-Time: 1.1.1970 01:00 //Edit: Nope! 1.1.1970 00:00
	}

	String Time::format(TimeFormat const& format) const
	{
		return format.Format(*this);
	}
	String Time::toString() const
	{
		return StringHelpers::ToString((uint32_t) this->hour) + ":" + StringHelpers::ToString((uint32_t) this->minute) + ":" + StringHelpers::ToString((uint32_t) this->second) + ":" + StringHelpers::ToString((uint32_t) this->millisecond);
	}

	Time Time::Now(Timezone const& tz)
	{
		return tz.getCurrentTime();
	}
	Time Time::FromJSTimestamp(JSTimestamp timestamp, Timezone const& tz)
	{
		static uint32_t secondMS = 1000;
		static uint32_t minuteMS = 60 * secondMS;
		static uint32_t hourMS = 60 * minuteMS;
		static uint32_t dayMS = 24 * hourMS;

		Time buffer(tz);

		if (timestamp < 0)
		{
			timestamp *= -1;
			timestamp %= dayMS;

			timestamp = dayMS - timestamp;
		}
		else
			timestamp %= dayMS;

		buffer.hour = (Hours) (timestamp / hourMS);
		timestamp -= (buffer.hour * hourMS);

		//EDIT this one too - not 1.1.1970 01:00, but 1.1.1970 00:00!
		//buffer.hour++;
		//if (buffer.hour > 23)
		//	buffer.hour = 24 - buffer.hour;

		buffer.minute = (Minutes) (timestamp / minuteMS);
		timestamp -= (buffer.minute * minuteMS);

		buffer.second = (Seconds) (timestamp / secondMS);
		timestamp -= (buffer.second * secondMS);

		buffer.millisecond = (Time::Milliseconds) timestamp;

		return buffer;
	}
}
