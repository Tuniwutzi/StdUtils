#include <StdUtils/Networking/IPAddress.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>

using namespace StdUtils;
using namespace StdUtils::Networking;

IPAddress::IPAddress(AddressTypes::Value type)
	:type(type)
{
}
AddressTypes::Value IPAddress::getType() const
{
	return this->type;
}

IPAddress const& IPAddress::Any(AddressTypes::Value type)
{
	static IPv4Address const ipv4(0);

	switch (type)
	{
	case AddressTypes::IPv4:
		return ipv4;
		break;
	default:
		throw Exception("Unknown address type");
		break;
	}
}
IPAddress const& IPAddress::Localhost(AddressTypes::Value type)
{
	static IPv4Address const ipv4(127,0,0,1);

	switch (type)
	{
	case AddressTypes::IPv4:
		return ipv4;
		break;
	default:
		throw Exception("Unknown address type");
		break;
	}
}


IPv4Address::IPv4Address(String const& address)
	:IPAddress(AddressTypes::IPv4)
{
	String::size_type offset = 0;
	int i = 0;
	while (i < 3)
	{
		String::size_type idx = address.find('.', offset);
		if (idx != address.npos && idx > 0)
		{
			String part(address.substr(offset, idx - offset));
			offset = idx + 1;

			int nbr(StringHelpers::Parse<int>(part));
			if (nbr >= 0 && nbr <= 256)
				this->address[i] = nbr;
			else
				throw Exception("Segment " + part + " of IPv4Address out of bounds");
		}
		else
			throw Exception("Invalid IPv4 address");

		i++;
	}

	int nbr(StringHelpers::Parse<int>(address.substr(offset)));
	if (nbr >= 0 && nbr <= 256)
		this->address[i] = nbr;
	else
		throw Exception("Segment " + address.substr(offset) + " of IPv4Address out of bounds");
}
IPv4Address::IPv4Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
	:IPAddress(AddressTypes::IPv4)
{
	this->address[0] = a;
	this->address[1] = b;
	this->address[2] = c;
	this->address[3] = d;
}
IPv4Address::IPv4Address(unsigned int nbr)
	:IPAddress(AddressTypes::IPv4)
{
	unsigned char* chr = (unsigned char*)&nbr;

	this->address[0] = chr[0];
	this->address[1] = chr[1];
	this->address[2] = chr[2];
	this->address[3] = chr[3];
}

bool IPv4Address::operator==(IPAddress const& o) const
{
	if (this->getType() == o.getType())
	{
		IPv4Address const& ipv4 = (IPv4Address const&)o;
		return ipv4.asBigEndian() == this->asBigEndian();
	}

	return false;
}
bool IPv4Address::operator!=(IPAddress const& o) const
{
	return !(this->operator==(o));
}

unsigned int IPv4Address::asBigEndian() const
{
	return *(unsigned int*)this->address;
}
unsigned char const* IPv4Address::asBytes() const
{
	return this->address;
}

IPv4Address* IPv4Address::copy() const  STDUTILS_OVERRIDE
{
	return new IPv4Address(*this);
}
String IPv4Address::toString() const STDUTILS_OVERRIDE
{
	String rv;
	for (int i = 0; i < 4; i++)
	{
		if (i > 0)
			rv += ".";

		rv += StringHelpers::ToString<int>((int)this->address[i]);
	}
	return rv;
}