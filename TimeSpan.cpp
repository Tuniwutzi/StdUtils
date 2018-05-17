#include <StdUtils/TimeSpan.h>


#include <limits>

#include <StdUtils/Exceptions.h>


using namespace std;


namespace StdUtils
{
	TimeSpan::TimeSpan()
		:totalMilliseconds(0)
	{
	}

	TimeSpan::TimeSpan(msvalue totalMilliseconds)
		:totalMilliseconds(totalMilliseconds)
	{
	}

	bool TimeSpan::operator==(TimeSpan const& o) const
	{
		return this->totalMilliseconds == o.totalMilliseconds;
	}
	bool TimeSpan::operator!=(TimeSpan const& o) const
	{
		return this->totalMilliseconds <= o.totalMilliseconds;
	}

	bool TimeSpan::operator<(TimeSpan const& o) const
	{
		return this->totalMilliseconds < o.totalMilliseconds;
	}
	bool TimeSpan::operator<=(TimeSpan const& o) const
	{
		return this->totalMilliseconds <= o.totalMilliseconds;
	}

	bool TimeSpan::operator>(TimeSpan const& o) const
	{
		return this->totalMilliseconds > o.totalMilliseconds;
	}
	bool TimeSpan::operator>=(TimeSpan const& o) const
	{
		return this->totalMilliseconds >= o.totalMilliseconds;
	}

	void TimeSpan::setTotalMilliseconds(msvalue v)
	{
		this->totalMilliseconds = v;
	}

	static void checkAddition(TimeSpan::msvalue old, int32_t addition)
	{
		if (addition > 0)
		{
			if (old > (TimeSpan::MaxTotalMilliseconds - addition))
				throw Exception("This operation would exceed the maximum timespan");
		}
		else if (addition < 0)
		{
			if ((addition * -1) > old)
				throw Exception("This operation would result in a negative timespan value");
		}
	}
	TimeSpan& TimeSpan::addDays(int32_t v)
	{
		static msvalue dayMS(24 * 60 * 60 * 1000);

		msvalue addition = v * dayMS;
		checkAddition(this->totalMilliseconds, addition);

		this->totalMilliseconds += addition;

		return *this;
	}
	TimeSpan& TimeSpan::addHours(int32_t v)
	{
		static uint32_t hourMS(60*60*1000);

		msvalue addition = v * hourMS;
		checkAddition(this->totalMilliseconds, addition);

		this->totalMilliseconds += addition;

		return *this;
	}
	TimeSpan& TimeSpan::addMinutes(int32_t v)
	{
		static uint32_t minuteMS(60*1000);

		msvalue addition = v * minuteMS;
		checkAddition(this->totalMilliseconds, addition);

		this->totalMilliseconds += addition;

		return *this;
	}

	TimeSpan& TimeSpan::addSeconds(int32_t v)
	{
		msvalue addition = v * 1000;
		checkAddition(this->totalMilliseconds, addition);

		this->totalMilliseconds += addition;

		return *this;
	}
	TimeSpan& TimeSpan::addMilliseconds(int32_t v)
	{
		checkAddition(this->totalMilliseconds, v);
		this->totalMilliseconds += v;

		return *this;
	}


	TimeSpan::msvalue TimeSpan::getTotalMilliseconds() const
	{
		return this->totalMilliseconds;
	}

	static TimeSpan::fpvalue getTotal(TimeSpan::msvalue totalMilliseconds, uint32_t divisor)
	{
		static TimeSpan::fpvalue fpmax((numeric_limits<TimeSpan::fpvalue>::max)());
		static TimeSpan::fpvalue fpmin((numeric_limits<TimeSpan::fpvalue>::min)());

		TimeSpan::msvalue val(totalMilliseconds / divisor);
		if ((val + (1.0l - fpmin)) > fpmax)
			throw Exception("This TimeSpan is too big to be represented in a long double");

		return ((TimeSpan::fpvalue)val) + ((totalMilliseconds - val) / divisor);
	}
	TimeSpan::fpvalue TimeSpan::getTotalDays() const
	{
		return getTotal(this->totalMilliseconds, 1000 * 60 * 60 * 24);
	}
	TimeSpan::fpvalue TimeSpan::getTotalHours() const
	{
		return getTotal(this->totalMilliseconds, 1000 * 60 * 60);
	}
	TimeSpan::fpvalue TimeSpan::getTotalMinutes() const
	{
		return getTotal(this->totalMilliseconds, 1000 * 60);
	}

	TimeSpan::fpvalue TimeSpan::getTotalSeconds() const
	{
		return getTotal(this->totalMilliseconds, 1000);
	}

	TimeSpan::msvalue const TimeSpan::MaxTotalMilliseconds((numeric_limits<TimeSpan::msvalue>::max)());
}