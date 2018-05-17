#ifndef STDUTILS_DATETIME
#define STDUTILS_DATETIME


#include <StdUtils/DateTimeFormats.h>
#include <StdUtils/TimeSpan.h>
#include <StdUtils/Timezone.h>


namespace StdUtils
{
	//TODO!: Timezone aus Date und Time entfernen und nur in DateTime integrieren; Timezone ist bedeutungslos fuer nur Date oder Time
	class STDUTILS_DLLCONTENT Date
	{
	public:
		typedef int Year;
		typedef unsigned short Month;
		typedef unsigned short Day;

		typedef unsigned short Weekday;

	private:
		Timezone const* timezone;

		Year year;
		Month month;
		Day day;

	public:
		explicit Date(Timezone const& tz = Timezone::Local());

	public:
		bool operator==(Date const&) const;
		bool operator!=(Date const&) const;

		bool operator<(Date const&) const;
		bool operator>(Date const&) const;
		bool operator<=(Date const&) const;
		bool operator>=(Date const&) const;
		
		//Definitely possible, but maybe not sensible
		//Date operator+(TimeSpan const&) const;
		//Date operator-(TimeSpan const&) const;

		//Date& operator+=(TimeSpan const&);
		//Date& operator-=(TimeSpan const&);

    public:
		Year getYear() const;
		Month getMonth() const;
		Day getDay() const;

		void setYear(Year year);
		void setMonth(Month month);
		void setDay(Day day);

		Weekday getWeekday() const;

		Timezone const& getTimezone() const;

		String format(DateFormat const& format) const;
        String toString() const;
		JSTimestamp getJSTimestamp() const;

	public:
		static Date Today(Timezone const& tz = Timezone::Local());
		static Date Parse(String const& raw, DateFormat const& format, Timezone const& tz = Timezone::Local());
        static Date Parse(String const& raw, Timezone const& tz = Timezone::Local());
		static Date FromJSTimestamp(JSTimestamp timestamp, Timezone const& tz = Timezone::Local());
	};
	class STDUTILS_DLLCONTENT Time
	{
	public:
		typedef unsigned char Hours;
		typedef unsigned char Minutes;
		typedef unsigned char Seconds;
		typedef uint16_t Milliseconds;

	private:
		Timezone const* timezone;

		Hours hour;
		Minutes minute;
		Seconds second;
		Milliseconds millisecond;

	public:
		explicit Time(Timezone const& tz = Timezone::Local());

	public:
		bool operator==(Time const&) const;
		bool operator!=(Time const&) const;

		bool operator<(Time const&) const;
		bool operator>(Time const&) const;
		bool operator<=(Time const&) const;
		bool operator>=(Time const&) const;

		//Definitely possible, but maybe not sensible
		//Time operator+(TimeSpan const&) const;
		//Time operator-(TimeSpan const&) const;

		//Time& operator+=(TimeSpan const&);
		//Time& operator-=(TimeSpan const&);
        
    public:
		Hours getHour() const;
		Minutes getMinute() const;
		Seconds getSecond() const;
		Milliseconds getMillisecond() const;

		void setHour(Hours hour);
		void setMinute(Minutes minute);
		void setSecond(Seconds second);
		void setMillisecond(Milliseconds millisecond);

		Timezone const& getTimezone() const;

		String format(TimeFormat const& format) const;
        String toString() const;
		JSTimestamp getJSTimestamp() const;

	public:
		static Time Now(Timezone const& tz = Timezone::Local());
		static Time Parse(String const& raw, TimeFormat const& format, Timezone const& tz = Timezone::Local());
        static Time Parse(String const& raw, Timezone const& tz = Timezone::Local());
		static Time FromJSTimestamp(JSTimestamp timestamp, Timezone const& tz = Timezone::Local());
	};

	class STDUTILS_DLLCONTENT DateTime
	{
	private:
		Date date;
		Time time;

	public:
		explicit DateTime(Timezone const& tz = Timezone::Local());

	public:
		bool operator==(DateTime const&) const;
		bool operator!=(DateTime const&) const;

		bool operator<(DateTime const&) const;
		bool operator>(DateTime const&) const;
		bool operator<=(DateTime const&) const;
		bool operator>=(DateTime const&) const;

		DateTime operator+(TimeSpan const&) const;
		DateTime operator-(TimeSpan const&) const;

		DateTime& operator+=(TimeSpan const&);
		DateTime& operator-=(TimeSpan const&);

    public:
		Date const& getDate() const;
		Date& getDate();

		void setDate(Date const& d);

		Time const& getTime() const;
		Time& getTime();

		Timezone const& getTimezone() const;

		void setTime(Time const&);

		String format(DateTimeFormat const& format) const;
        String toString() const;
		JSTimestamp getJSTimestamp() const;

	public:
		static DateTime Now(Timezone const& tz = Timezone::Local());
		static DateTime Parse(String const& raw, DateTimeFormat const& format, Timezone const& tz = Timezone::Local());
        static DateTime Parse(String const& raw, Timezone const& tz = Timezone::Local());
		static DateTime FromJSTimestamp(JSTimestamp timestamp, Timezone const& tz = Timezone::Local());
	};
}

#endif
