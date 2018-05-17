#include <StdUtils/Networking/Http/HttpBody.h>


#include <StdUtils/Exceptions.h>


using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;


HttpBody::HttpBody(NetworkStream& socket, bool receiving)
	:socket(socket),
	closed(false),
	receiving(receiving)
{
}

void HttpBody::assertRead() const
{
	if (!this->isOpen() || !this->isReadingBody())
		throw Exception("HttpBody is not available for reading");
}
void HttpBody::assertWrite() const
{
	if (!this->isOpen() || !this->isWritingBody())
		throw Exception("HttpBody is not available for writing");
}

void HttpBody::close()
{
	if (!this->isOpen())
		throw Exception("HttpBody is already closed");
	this->closed = true;
}

bool HttpBody::isOpen() const
{
	return !this->closed;
}

bool HttpBody::isReadingBody() const
{
	return this->receiving;
}
bool HttpBody::isWritingBody() const
{
	return !this->receiving;
}