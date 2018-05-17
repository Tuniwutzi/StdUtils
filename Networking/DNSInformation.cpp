#include <StdUtils/Networking/DNSInformation.h>


#include <StdUtils/Exceptions.h>


using namespace std;


namespace StdUtils
{
	namespace Networking
	{
		DNSInformation::DNSInformation(String const& hostName, vector<IPAddress const*> const& addresses)
		{
			this->fill(hostName, addresses);
		}
		DNSInformation::DNSInformation(DNSInformation const& original)
			:hostName(original.hostName)
		{
			this->destruct();
			this->fill(original.hostName, original.allAddresses);
		}
		DNSInformation::~DNSInformation()
		{
		}

		DNSInformation& DNSInformation::operator=(DNSInformation const& original)
		{
			this->destruct();
			this->fill(original.hostName, original.allAddresses);
			return *this;
		}

		void DNSInformation::destruct()
		{
			this->hostName.clear();
			this->allAddresses.clear();
			this->ipv4Addresses.clear();
		}
		void DNSInformation::fill(String const& hostName, vector<IPAddress const*> const& addresses)
		{
			this->hostName = hostName;
			for (vector<IPAddress const*>::const_iterator it = addresses.begin(); it != addresses.end(); it++)
			{
				switch ((*it)->getType())
				{
				case AddressTypes::IPv4:
					this->ipv4Addresses.push_back(*(IPv4Address const*)*it);

					//Geht nicht hier, da der Vektor bei nachfolgenden Vergroesserungen sonst die Speicherstelle verschiebt
					//this->allAddresses.push_back(&*this->ipv4Addresses.rbegin());
					break;
				default:
					throw Exception("Unknown address type");
					break;
				}
			}

			for (vector<IPv4Address>::const_iterator it = this->ipv4Addresses.begin(); it != this->ipv4Addresses.end(); it++)
				this->allAddresses.push_back(&*it);
		}

		String const& DNSInformation::getHostName() const
		{
			return this->hostName;
		}
		IPAddress const& DNSInformation::getFirstAddress() const
		{
			if (this->allAddresses.size() > 0)
				return **this->allAddresses.begin();
			else
				throw Exception("No addresses available");
		}
		vector<IPAddress const*> const& DNSInformation::getAddresses() const
		{
			return this->allAddresses;
		}
	}
}