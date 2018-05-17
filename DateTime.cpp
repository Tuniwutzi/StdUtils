#include <StdUtils/DateTime.h>
#include <StdUtils/Exceptions.h>

namespace StdUtils
{
	DateTime::DateTime(Timezone const& tz)
		: date(tz),
		  time(tz)
	{
	}

	bool DateTime::operator ==(DateTime const& dt) const
	    {
		if (&dt.getTimezone() == &this->getTimezone())
			return this->date == dt.date && this->time == dt.time;
		else
		{
			DateTime buf(this->getTimezone().convert(dt));
			return this->operator ==(buf);
		}
	}
	bool DateTime::operator !=(DateTime const& dt) const
	    {
		return !this->operator ==(dt);
	}

	bool DateTime::operator<(DateTime const& other) const
	{
		if (this->date <= other.date)
			return this->date < other.date || this->time < other.time;
		
		return false;
	}

	DateTime DateTime::operator+(TimeSpan const& span) const
	{
		JSTimestamp meMS(this->getJSTimestamp());
		uint64_t spanMS(span.getTotalMilliseconds());
		if ((INT64_MAX - meMS) < spanMS)
			throw Exception("New DateTime value out of bounds");

		meMS += spanMS;

		return DateTime::FromJSTimestamp(meMS, this->getTimezone());
	}


	Date const& DateTime::getDate() const
	{
		return this->date;
	}
	Date& DateTime::getDate()
	{
		return this->date;
	}

	void DateTime::setDate(Date const& dt)
	{
		if (dt.getTimezone() == this->getTimezone())
			this->date = dt;
		else
			throw Exception("Date must not have different timezone than DateTime");
	}

	Time const& DateTime::getTime() const
	{
		return this->time;
	}
	Time& DateTime::getTime()
	{
		return this->time;
	}

	void DateTime::setTime(Time const& t)
	{
		if (t.getTimezone() == this->getTimezone())
			this->time = t;
		else
			throw Exception("Time must not have different timezone than DateTime");
	}

	Timezone const& DateTime::getTimezone() const
	{
		return this->date.getTimezone();
	}

	JSTimestamp DateTime::getJSTimestamp() const
	{
		return this->date.getJSTimestamp() + this->time.getJSTimestamp();
	}

	String DateTime::format(DateTimeFormat const& format) const
	    {
		return format.Format(*this);
	}
	String DateTime::toString() const
	{
		return this->date.toString() + " " + this->time.toString() + " " + this->getTimezone().toString();
	}

	DateTime DateTime::Parse(String const& raw, DateTimeFormat const& format, Timezone const& tz)
	{
		DateTime buffer(tz);
		format.Parse(buffer, raw);
		return buffer;
	}

	DateTime DateTime::Now(Timezone const& tz)
	{
		return tz.now();
	}

	DateTime DateTime::FromJSTimestamp(JSTimestamp timestamp, Timezone const& tz)
	{
		DateTime rv(tz);

		rv.setDate(Date::FromJSTimestamp(timestamp, tz));
		rv.setTime(Time::FromJSTimestamp(timestamp - rv.getDate().getJSTimestamp(), tz));

		return rv;
	}
}
