#include <StdUtils/Networking/Endpoint.h>

#include <StdUtils/Exceptions.h>

using namespace StdUtils;
using namespace StdUtils::Networking;

Endpoint::Endpoint()
	:address(NULL),
	port(Endpoint::AnyPort)
{
}
Endpoint::Endpoint(IPAddress const& address, unsigned short port)
	:address(address.copy()),
	port(port)
{
	//switch (address.getType())
	//{
	//case AddressTypes::IPv4:
	//	this->address = new IPv4Address((IPv4Address const&)address);
	//	break;
	//default:
	//	throw Exception("AddressType not supported");
	//	break;
	//}
}
Endpoint::Endpoint(Endpoint const& ep)
	:port(ep.port),
	address(ep.isNull() ? NULL : ep.getAddress().copy())
{
	//if (!ep.isNull())
	//{
	//	switch (ep.address->getType())
	//	{
	//	case AddressTypes::IPv4:
	//		this->address = new IPv4Address(*(IPv4Address const*)ep.address);
	//		break;
	//	default:
	//		throw Exception("Invalid endpoint instance");
	//		break;
	//	}
	//}
}
Endpoint& Endpoint::operator=(Endpoint const& ep)
{
	this->port = ep.port;

	delete this->address;
	this->address = ep.isNull() ? NULL : ep.getAddress().copy();

	//if (!ep.isNull())
	//{
	//	switch (ep.address->getType())
	//	{
	//	case AddressTypes::IPv4:
	//		this->address = new IPv4Address(*(IPv4Address const*)ep.address);
	//		break;
	//	default:
	//		throw Exception("Invalid endpoint instance");
	//		break;
	//	}
	//}
	return *this;
}

Endpoint::~Endpoint()
{
	delete this->address;
}

bool Endpoint::isNull() const
{
	return this->address == NULL;
}
IPAddress const& Endpoint::getAddress() const
{
	if (!this->isNull())
		return *this->address;
	else
		throw Exception("Endpoint instance is null");
}
unsigned short Endpoint::getPort() const
{
	if (!this->isNull())
		return this->port;
	else
		throw Exception("Endpoint instance is null");
}

const unsigned short Endpoint::AnyPort(0);