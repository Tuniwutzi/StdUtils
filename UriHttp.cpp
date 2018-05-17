#include <StdUtils/Uri.h>

#include <StdUtils/StringHelpers.h>

namespace StdUtils
{
	Uri<UriFormats::Http>::Uri()
		: scheme("http")
	{
	}
	Uri<UriFormats::Http>::Uri(String const& uri)
	{
		Uri<UriFormats::Unknown> u(uri);
		this->scheme = u.getScheme();

		String schemeSpecificPart(u.getSchemeSpecificPart());
		String::size_type offset(0);

		if (schemeSpecificPart.find("//", offset) == offset)
		{
			this->authority = Authority(schemeSpecificPart, offset);
		}
		//else: Keine authority, alles null

		this->parseRessource(schemeSpecificPart, offset);
	}
	Uri<UriFormats::Http>::Uri(String const& scheme, String const& schemeSpecificPart)
		: scheme(scheme)
	{
		String::size_type offset(0);
		
		if (schemeSpecificPart.find("//", offset) == offset)
		{
			this->authority = Authority(schemeSpecificPart, offset);
		}

		this->parseRessource(schemeSpecificPart, offset);
	}

	void Uri<UriFormats::Http>::parseRessource(String const& tmp, String::size_type& offset)
	{
		String::size_type qss = tmp.find("?", offset);
		if (qss != tmp.npos)
		{
			this->ressource = tmp.substr(offset, qss - offset);
			offset = qss;
		}
		else
		{
			String::size_type fragmentIndex(tmp.find("#", offset));
			if (fragmentIndex != tmp.npos)
			{
				this->ressource = tmp.substr(offset, fragmentIndex - offset);
				offset = fragmentIndex;
			}
			else
			{
				this->ressource = tmp.substr(offset);
				offset = tmp.size();
			}
		}

		qss = tmp.find("?", offset);
		if (qss == offset)
		{
			String::size_type fragmentIndex = tmp.find("#", offset);
			if (fragmentIndex != tmp.npos)
			{
				this->query = Query(tmp.substr(offset, fragmentIndex - offset));
				offset = fragmentIndex;
			}
			else
			{
				this->query = Query(tmp.substr(offset));
				offset = tmp.size();
			}
		}
		else if (qss != tmp.npos)
			throw Exception("Unexpected ? in uri");

		if (offset < tmp.size())
			this->fragment = tmp.substr(offset);
	}

	String const& Uri<UriFormats::Http>::getScheme() const
	{
		return this->scheme;
	}

	Nullable<String> Uri<UriFormats::Http>::getHost() const
	{
		if (this->authority.hasValue())
			return this->authority.getValue().getHost();
		else
			return Nullable<String>();
	}
	void Uri<UriFormats::Http>::setHost(Nullable<String> const& h)
	{
		if (h.isNull())
		{
			if (!this->authority.isNull())
			{
				if (this->authority->getUser().hasValue() || this->authority->getPassword().hasValue() || this->authority->getPort().hasValue())
					throw Exception("Must set username, password and port to null before host.");
			}
			this->authority.setNull();
		}
		else
		{
			if (this->authority.isNull())
				this->authority = Authority(h.getValue());
		}
	}

	Nullable<uint16_t> Uri<UriFormats::Http>::getPort() const
	{
		if (this->authority.hasValue())
			return this->authority.getValue().getPort();
		else
			return Nullable<uint16_t>();
	}

	Nullable<Query>& Uri<UriFormats::Http>::getQuery()
	{
		return this->query;
	}
	Nullable<Query> const& Uri<UriFormats::Http>::getQuery() const
	{
		return this->query;
	}
	void Uri<UriFormats::Http>::setQuery(Nullable<Query> const& q)
	{
		this->query = q;
	}

	Nullable<String> const& Uri<UriFormats::Http>::getFragment() const
	{
		return this->fragment;
	}
	void Uri<UriFormats::Http>::setFragment(Nullable<String> const& f)
	{
		this->fragment = f;
	}

	String const& Uri<UriFormats::Http>::getPath() const
	{
		return this->ressource;
	}
	void Uri<UriFormats::Http>::setPath(String const& res)
	{
		this->ressource = res;
	}

	bool Uri<UriFormats::Http>::isRelative() const
	{
		return !this->authority.hasValue();
	}

	String Uri<UriFormats::Http>::toString() const
	{
		return this->scheme + ":" + (this->authority.hasValue() ? this->authority.getValue().toString() : "") + this->toRelativeString(false);
	}
	String Uri<UriFormats::Http>::toRelativeString(bool includeScheme) const
	{
		return (includeScheme ? this->scheme + ":" : "") + this->ressource + (
		    this->query.hasValue() ? this->query.getValue().toString() : "") + (
		    this->fragment.hasValue() ? this->fragment.getValue() : "");
	}
}
