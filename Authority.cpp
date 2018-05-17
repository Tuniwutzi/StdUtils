#include <StdUtils/Authority.h>


#include <StdUtils/StringHelpers.h>


namespace StdUtils
{
	Authority::Authority(String const& host)
		:host(host)
	{
	}
	Authority::Authority(String const& tmp, String::size_type& offset)
	{
		String::size_type idx;

		if (tmp.find("//", offset) == offset)
		{
			//doppelslash leitet authority ([username[:password]@]host) ein
			offset += 2;

			if ((idx = tmp.find("@", offset)) != tmp.npos)
			{
				String::size_type colon(tmp.find(":", offset));
				if (colon != tmp.npos && colon < idx)
				{
					this->user = tmp.substr(offset, colon - offset);
					this->password = tmp.substr(colon + 1, idx - (colon + 1));
				}
				else
				{
					this->user = tmp.substr(offset, idx - offset);
				}
				offset = idx + 1;
			}

			String::size_type slash(tmp.find("/", offset));
			String::size_type query(tmp.find("?", offset));
			String::size_type fragment(tmp.find("#", offset));
			//das folgende konstrukt sucht den kleinsten nicht-npos-index raus...
			if (slash != tmp.npos)
			{
				if (query != tmp.npos)
				{
					if (fragment != tmp.npos)
						idx = slash < query ? slash < fragment ? slash : fragment : query < fragment ? query : fragment; //Den kleinsten Index der drei nehmen
					else
						idx = slash < query ? slash : query;
				}
				else
				{
					if (fragment != tmp.npos)
						idx = slash < fragment ? slash : fragment;
					else
						idx = slash;
				}
			}
			else if (query != tmp.npos)
			{
				if (fragment != tmp.npos)
					idx = query < fragment ? query : fragment;
				else
					idx = query;
			}
			else if (fragment != tmp.npos)
				idx = fragment;
			else
				idx = tmp.size(); //Alles gehoert zur authority

			//idx = index, an dem die authority endet

			String::size_type colon(tmp.find(":", offset));
			if (colon != tmp.npos && colon < idx)
			{
				this->host = tmp.substr(offset, colon - offset);
				this->port = StringHelpers::Parse<uint16_t>(tmp.substr(colon + 1, idx - (colon + 1)));
			}
			else
				this->host = tmp.substr(offset, idx - offset);

			offset = idx;
		}
		else
			throw Exception("String does not start with valid authority");
	}

	Nullable<String> const& Authority::getUser() const
	{
		return this->user;
	}
	Nullable<String>& Authority::getUser()
	{
		return this->user;
	}
	void Authority::setUser(Nullable<String> const& user)
	{
		this->user = user;
	}

	Nullable<String> const& Authority::getPassword() const
	{
		return this->password;
	}
	Nullable<String>& Authority::getPassword()
	{
		return this->password;
	}
	void Authority::setPassword(Nullable<String> const& password)
	{
		this->password = password;
	}

	String const& Authority::getHost() const
	{
		return this->host;
	}
	String& Authority::getHost()
	{
		return this->host;
	}
	void Authority::setHost(String const& host)
	{
		this->host = host;
	}

	Nullable<uint16_t> const& Authority::getPort() const
	{
		return this->port;
	}
	Nullable<uint16_t>& Authority::getPort()
	{
		return this->port;
	}
	void Authority::setPort(Nullable<uint16_t> const& port)
	{
		this->port = port;
	}

	String Authority::toString() const
	{
		return String("//") + (this->user.hasValue() ? this->user.getValue() + (this->password.hasValue() ? String(":") + this->password.getValue() : "") + "@" : "") + this->host + (this->port.hasValue() ? String(":") + StringHelpers::ToString(this->port.getValue()) : "");
	}
}