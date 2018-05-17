#ifndef STDUTILS_DOMAINNAMESYSTEM_H
#define STDUTILS_DOMAINNAMESYSTEM_H

#include <StdUtils/Networking/DNSInformation.h>
#include <StdUtils/Networking/IPAddress.h>

namespace StdUtils
{
	namespace Networking
	{
		class DomainNameSystem
		{
			STDUTILS_PREVENT_ASSIGNMENT(DomainNameSystem);
			STDUTILS_PREVENT_COPY(DomainNameSystem);
		private:
			DomainNameSystem();

		public:
			static DNSInformation Resolve(String const& hostName);
			static DNSInformation ReverseLookup(IPAddress const& address);
		};
	}
}

#endif