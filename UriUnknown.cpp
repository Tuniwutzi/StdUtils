#include <StdUtils/Uri.h>


#include <StdUtils/StringHelpers.h>


namespace StdUtils
{
	Uri<UriFormats::Unknown>::Uri(String const& uri)
	{
		String::size_type pos(uri.find(":"));
		if (pos != uri.npos && pos != 0)
		{
			this->scheme = uri.substr(0, pos);
			this->schemeSpecific = uri.substr(pos + 1);
		}
		else
			throw Exception("Invalid uri format; could not identify scheme");
	}
	Uri<UriFormats::Unknown>::Uri(String const& scheme, String const& schemeSpecific)
		:scheme(scheme),
		schemeSpecific(schemeSpecific)
	{
	}
	String const& Uri<UriFormats::Unknown>::getScheme() const
	{
		return this->scheme;
	}
	String const& Uri<UriFormats::Unknown>::getSchemeSpecificPart() const
	{
		return this->schemeSpecific;
	}
	String Uri<UriFormats::Unknown>::toString() const
	{
		return this->scheme + ":" + this->schemeSpecific;
	}

	UriFormats::Value Uri<UriFormats::Unknown>::GetFormat(String const& uri)
	{
		static struct
		{
			String Name;
			UriFormats::Value Format;
		} formats[] =
		{
			{ "http", UriFormats::Http },
			{ "https", UriFormats::Http },
			{ "ws", UriFormats::WebSocket },
			{ "wss", UriFormats::WebSocket }
		};

		Uri<UriFormats::Unknown> ur(uri);
		String scheme(StringHelpers::ToLower(ur.getScheme()));
		for (int i = 0; i < STDUTILS_ARRAYCOUNT(formats); i++)
		{
			if (scheme == formats[i].Name)
				return formats[i].Format;
		}
		return UriFormats::Unknown;
	}
}