#ifndef STDUTILS_URI_H
#define STDUTILS_URI_H


#include <StdUtils/Base.h>
#include <StdUtils/Nullable.h>

#include <StdUtils/Query.h>
#include <StdUtils/Authority.h>


namespace StdUtils
{
	STDUTILS_START_ENUM(UriFormats)
	{
		Unknown,
			Http,
			WebSocket = Http
	}
	STDUTILS_END_ENUM(UriFormats);

	template<UriFormats::Value F = UriFormats::Unknown>
	class Uri;

	//TODO: Kann noch mehr leisten. Authority und query string scheinen bspw. eindeutig definiert zu sein, wenn auch nicht in jeder Uri vorhanden.
	template<>
	class Uri<UriFormats::Unknown>
	{
	private:
		String scheme;
		String schemeSpecific;

	public:
		explicit Uri(String const&);
		explicit Uri(String const& scheme, String const& schemeSpecific);

	public:
		String const& getScheme() const;
		String const& getSchemeSpecificPart() const;

		String toString() const;

	public:
		static UriFormats::Value GetFormat(String const& uri);
	};

	template<>
	class Uri<UriFormats::Http>
	{
	private:
		String scheme; //scheme != format
		Nullable<Authority> authority; //wenn null: relative uri
		//TODO?: In path umbenennen
		String ressource; //nicht nullable, da kein unterschied zwischen ressource == "" und ressource == null
		Nullable<Query> query;
		Nullable<String> fragment;

	public:
		Uri();
		explicit Uri(String const& absoluteUri);
		explicit Uri(String const& scheme, String const& relativeOrAbsoluteUri);

	private:
		void parseRessource(String const&, String::size_type&);

	public:
		String const& getScheme() const;
		void setScheme(String const&);

		Nullable<String> getHost() const;
		void setHost(Nullable<String> const&);

		Nullable<uint16_t> getPort() const;
		void setPort(Nullable<uint16_t> const&);

		String const& getPath() const;
		void setPath(String const&);

		Nullable<Query>& getQuery();
		Nullable<Query> const& getQuery() const;
		void setQuery(Nullable<Query> const&);

		Nullable<String> const& getFragment() const;
		void setFragment(Nullable<String> const&);

		bool isRelative() const;

		String toString() const;
		String toRelativeString(bool includeScheme = false) const;
	};
}

#endif
