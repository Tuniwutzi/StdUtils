#ifndef STDUTILS_TIMESPAN_H
#define STDUTILS_TIMESPAN_H


#include <StdUtils/Base.h>


namespace StdUtils
{
	class TimeSpan
	{
	public:
		typedef uint32_t msvalue;
		typedef long double fpvalue;

	private:
		msvalue totalMilliseconds;

	public:
		TimeSpan();
		TimeSpan(msvalue totalMilliseconds);

	public:
		bool operator==(TimeSpan const&) const;
		bool operator!=(TimeSpan const&) const;

		bool operator<(TimeSpan const&) const;
		bool operator<=(TimeSpan const&) const;

		bool operator>(TimeSpan const&) const;
		bool operator>=(TimeSpan const&) const;

	public:
		void setTotalMilliseconds(msvalue);

		TimeSpan& addDays(int32_t);
		TimeSpan& addHours(int32_t);
		TimeSpan& addMinutes(int32_t);
		TimeSpan& addSeconds(int32_t);
		TimeSpan& addMilliseconds(int32_t);


		msvalue getTotalMilliseconds() const;

		fpvalue getTotalDays() const;
		fpvalue getTotalHours() const;
		fpvalue getTotalMinutes() const;
		fpvalue getTotalSeconds() const;

	public:
		static msvalue const MaxTotalMilliseconds;
	};
}

#endif