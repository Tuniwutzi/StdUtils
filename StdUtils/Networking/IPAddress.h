#ifndef STDUTILS_IPADDRESS_H
#define STDUTILS_IPADDRESS_H

#include <StdUtils/Base.h>

namespace StdUtils
{
	namespace Networking
	{
		STDUTILS_START_ENUM(AddressTypes)
		{
			IPv4
		}
		STDUTILS_END_ENUM(AddressTypes);

		class IPAddress
		{
		private:
			AddressTypes::Value type;

		public:
			explicit IPAddress(AddressTypes::Value type);
			virtual ~IPAddress() {}

		public:
			virtual bool operator==(IPAddress const&) const = 0;
			virtual bool operator!=(IPAddress const&) const = 0;

		public:
			AddressTypes::Value getType() const;

			virtual IPAddress* copy() const = 0;
			virtual String toString() const = 0;

		public:
			static IPAddress const& Any(AddressTypes::Value type = AddressTypes::IPv4);
			static IPAddress const& Localhost(AddressTypes::Value type = AddressTypes::IPv4);
		};

		class IPv4Address : public IPAddress
		{
		private:
			unsigned char address[4];

		public:
			explicit IPv4Address(String const& address);
			explicit IPv4Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
			explicit IPv4Address(unsigned int bigEndian);

		public:
			bool operator==(IPAddress const&) const STDUTILS_OVERRIDE;
			bool operator!=(IPAddress const&) const STDUTILS_OVERRIDE;

		public:
			unsigned int asBigEndian() const;
			unsigned char const* asBytes() const;

			IPv4Address* copy() const STDUTILS_OVERRIDE;
			String toString() const STDUTILS_OVERRIDE;
		};
	}
}

#endif