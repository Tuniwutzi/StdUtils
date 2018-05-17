#ifndef STDUTILS_AUTHORITY_H
#define STDUTILS_AUTHORITY_H

#include <StdUtils/Base.h>
#include <StdUtils/Nullable.h>

namespace StdUtils
{
	class Authority
	{
	private:
		Nullable<String> user;
		Nullable<String> password;
		String host;
		Nullable<uint16_t> port;

	public:
		explicit Authority(String const& host);
		explicit Authority(String const& raw, String::size_type& offset);

	public:
		Nullable<String> const& getUser() const;
		Nullable<String>& getUser();
		void setUser(Nullable<String> const&);

		Nullable<String> const& getPassword() const;
		Nullable<String>& getPassword();
		void setPassword(Nullable<String> const&);

		String const& getHost() const;
		String& getHost();
		void setHost(String const&);

		Nullable<uint16_t> const& getPort() const;
		Nullable<uint16_t>& getPort();
		void setPort(Nullable<uint16_t> const&);

		String toString() const;
	};
}

#endif