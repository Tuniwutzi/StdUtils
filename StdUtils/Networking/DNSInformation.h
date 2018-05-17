#ifndef STDUTILS_DNSINFORMATION_H
#define STDUTILS_DNSINFORMATION_H


#include <StdUtils/Networking/IPAddress.h>

#include <vector>


namespace StdUtils
{
	namespace Networking
	{
		class DNSInformation
		{
		private:
			String hostName;

			std::vector<IPAddress const*> allAddresses;
			std::vector<IPv4Address> ipv4Addresses;

		public:
			DNSInformation(String const&, std::vector<IPAddress const*> const&);
			DNSInformation(DNSInformation const&);
			~DNSInformation();

		public:
			DNSInformation& operator=(DNSInformation const&);

		private:
			void destruct();
			void fill(String const&, std::vector<IPAddress const*> const&);

		public:
			String const& getHostName() const;
			
			IPAddress const& getFirstAddress() const;
			std::vector<IPAddress const*> const& getAddresses() const;
		};
	}
}


#endif