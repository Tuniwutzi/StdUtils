#include <StdUtils/KnownDateTimeFormat.h>


#include <StdUtils/StringHelpers.h>
#include <StdUtils/DateTime.h>


namespace StdUtils
{
	static void getParts(String const& raw, char delimiter, int* first, int* second, int* third, int* fourth = NULL)
	{
		String::size_type idx = raw.find(delimiter);

		if (idx > 0)
		{
			*first = StringHelpers::Parse<int>(raw.substr(0, idx));

			String::size_type idx1 = raw.find(delimiter, idx + 1);

			if (idx1 > 0)
			{
				idx++;

				*second = StringHelpers::Parse<int>(raw.substr(idx, idx1 - idx));

				if (fourth)
				{
					double seconds = StringHelpers::Parse<double>(raw.substr(idx1 + 1));
					*third = (int)seconds;

					*fourth = (seconds - (*third)) * 1000;
				}
				else
					*third = StringHelpers::Parse<int>(raw.substr(idx1 + 1));

				return;
			}
		}

		throw Exception("Invalid format");
	}

	void ISO8601DateFormat::Parse(Date& buffer, String const& raw) const
	{
		int day, month, year;
		getParts(raw, '-', &year, &month, &day);

		if (month > 0 && day > 0)
		{
			buffer.setDay(day);
			buffer.setMonth(month);
			buffer.setYear(year);
		}
		else
			throw Exception("Value for month or day out of range");
	}
	String ISO8601DateFormat::Format(Date const& date) const
	{
		String rv(StringHelpers::ToString(date.getYear()));
		rv += "-";
		rv += StringHelpers::ToString(date.getMonth()) + "-" + StringHelpers::ToString(date.getDay());

		return rv;
	}

	DateFormat& ISO8601DateFormat::Instance()
	{
		static ISO8601DateFormat instance;
		return instance;
	}

	void ISO8601TimeFormat::Parse(Time& buffer, String const& raw) const
	{
		int hours, minutes, seconds, milliseconds;
		getParts(raw, ':', &hours, &minutes, &seconds, &milliseconds);

		buffer.setHour(hours);
		buffer.setMinute(minutes);
		buffer.setSecond(seconds);
		buffer.setMillisecond(milliseconds);
	}
	String ISO8601TimeFormat::Format(Time const& time) const
	{
		String rv(StringHelpers::ToString<int>(time.getHour()));
		rv += ":";
		rv += StringHelpers::ToString<int>(time.getMinute());
		rv += ":";
		rv += StringHelpers::ToString<int>(time.getSecond());

		return rv;
	}

	TimeFormat& ISO8601TimeFormat::Instance()
	{
		static ISO8601TimeFormat instance;
		return instance;
	}

	void ISO8601DateTimeFormat::Parse(DateTime& buffer, String const& raw) const
	{
		bool parsed = false;

		size_t index = raw.find(' ');
		if (index != raw.npos)
		{
			if (raw.find(' ', index + 1) == raw.npos) //make sure there is only one space
			{
				ISO8601DateFormat::Instance().Parse(buffer.getDate(), raw.substr(0, index));
				ISO8601TimeFormat::Instance().Parse(buffer.getTime(), raw.substr(index + 1));
				parsed = true;
			}
		}

		if (!parsed)
			throw Exception("Invalid format");
	}

	String ISO8601DateTimeFormat::Format(DateTime const& dateTime) const
	{
		return (ISO8601DateFormat::Instance().Format(dateTime.getDate()) + " " + ISO8601TimeFormat::Instance().Format(dateTime.getTime()));
	}

	DateTimeFormat& ISO8601DateTimeFormat::Instance()
	{
		static ISO8601DateTimeFormat instance;
		return instance;
	}
}