#ifndef STDUTILS_TIMEZONE_H
#define STDUTILS_TIMEZONE_H

#include <stdint.h>

#include <StdUtils/Base.h>

namespace StdUtils
{
	class Date;
	class Time;
	class DateTime;

	typedef int64_t JSTimestamp;
	class Timezone
	{
		STDUTILS_NOCOPY(Timezone);

	protected:
		Timezone() {}
	public:
		virtual ~Timezone() {}

	public:
		bool operator==(Timezone const& other) const;
		bool operator!=(Timezone const& other) const;

		virtual JSTimestamp operator-(Timezone const& other) const;

	public:
		virtual DateTime now() const = 0;

		virtual Date getCurrentDate() const;
		virtual Time getCurrentTime() const;
		virtual DateTime convert(DateTime const& other) const;

		virtual StdUtils::String const& toString() const = 0;

	public:
		static Timezone const& Local();
		static Timezone const& Utc();
	};
}

#endif
