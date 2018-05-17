#ifndef STDUTILS_KNOWNDATETIMEFORMAT_H
#define STDUTILS_KNOWNDATETIMEFORMAT_H

#include <StdUtils/DateTimeFormats.h>

namespace StdUtils
{
	STDUTILS_START_ENUM(KnownDateTimeFormats)
	{
		ISO8601 //TODO: vollstaendig implementieren, siehe http://www.cl.cam.ac.uk/~mgk25/iso-time.html
	}
	STDUTILS_END_ENUM(KnownDateTimeFormats);

	template<KnownDateTimeFormats::Value>
	class KnownDateFormat : public StdUtils::DateFormat
	{
	public:
		virtual ~KnownDateFormat() {}
	public:
		virtual StdUtils::String Format(StdUtils::Date const& date) const STDUTILS_OVERRIDE;
		virtual void Parse(StdUtils::Date& buffer, StdUtils::String const& raw) const STDUTILS_OVERRIDE;

	public:
		static StdUtils::DateFormat& Instance();
	};
	template<KnownDateTimeFormats::Value>
	class KnownTimeFormat : public StdUtils::TimeFormat
	{
	public:
		virtual ~KnownTimeFormat() {}
	public:
		virtual StdUtils::String Format(StdUtils::Time const& time) const STDUTILS_OVERRIDE;
		virtual void Parse(StdUtils::Time& buffer, StdUtils::String const& raw) const STDUTILS_OVERRIDE;

	public:
		static StdUtils::TimeFormat& Instance();
	};
	template<KnownDateTimeFormats::Value>
	class KnownDateTimeFormat : public StdUtils::DateTimeFormat
	{
	public:
		virtual ~KnownDateTimeFormat() {}

	public:
		virtual StdUtils::String Format(StdUtils::DateTime const& date) const STDUTILS_OVERRIDE;
		virtual void Parse(StdUtils::DateTime& buffer, StdUtils::String const& raw) const STDUTILS_OVERRIDE;

	public:
		static StdUtils::DateTimeFormat& Instance();
	};

	typedef KnownDateFormat<KnownDateTimeFormats::ISO8601> ISO8601DateFormat;
	typedef KnownTimeFormat<KnownDateTimeFormats::ISO8601> ISO8601TimeFormat;
	typedef KnownDateTimeFormat<KnownDateTimeFormats::ISO8601> ISO8601DateTimeFormat;
}

#endif