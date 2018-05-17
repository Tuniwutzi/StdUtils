#ifndef STDUTILS_DATETIMEFORMATS
#define STDUTILS_DATETIMEFORMATS

#include <StdUtils/Base.h>

namespace StdUtils
{
	class STDUTILS_DLLCONTENT Date;
	class STDUTILS_DLLCONTENT Time;
	class STDUTILS_DLLCONTENT DateTime;

	class STDUTILS_DLLCONTENT DateFormat
	{
	public:
		virtual ~DateFormat() {}
	public:
		virtual String Format(Date const& date) const = 0;
		virtual void Parse(Date& buffer, String const& raw) const = 0;
	};

	class STDUTILS_DLLCONTENT TimeFormat
	{
	public:
		virtual ~TimeFormat() {}
	public:
		virtual String Format(Time const& date) const = 0;
		virtual void Parse(Time& buffer, String const& raw) const = 0;
	};

	class STDUTILS_DLLCONTENT DateTimeFormat
	{
	public:
		virtual ~DateTimeFormat() {}
	public:
		virtual String Format(DateTime const& date) const = 0;
		virtual void Parse(DateTime& buffer, String const& raw) const = 0;
	};
}

#endif
