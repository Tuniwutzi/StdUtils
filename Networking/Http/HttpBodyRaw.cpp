#define NOMINMAX
#include <private/Networking/Http/HttpBodyRaw.h>


#include <StdUtils/Exceptions.h>

#include <algorithm>


using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpBodyRaw::HttpBodyRaw(NetworkStream& s, bool receiving, uint64_t contentLength)
	:HttpBody(s, receiving),
	transferred(0),
	contentLength(contentLength),
	knowsLength(true),
	tcpStream(dynamic_cast<TcpStream*>(&s))
{
}
HttpBodyRaw::HttpBodyRaw(NetworkStream& s, bool receiving)
	:HttpBody(s, receiving),
	transferred(0),
	contentLength(0),
	knowsLength(false),
	tcpStream(dynamic_cast<TcpStream*>(&s))
{
}
HttpBodyRaw::~HttpBodyRaw()
{
}


bool HttpBodyRaw::knowsWriteableBytes() const STDUTILS_OVERRIDE
{
	this->assertWrite();
	return this->knowsLength;
}
uint64_t HttpBodyRaw::getWriteableBytes() const STDUTILS_OVERRIDE
{
	if (this->knowsWriteableBytes())
		return this->contentLength - this->transferred;
	else
		throw Exception("HttpBody does not know writeable bytes");
}
void HttpBodyRaw::write(String const& str) STDUTILS_OVERRIDE
{
	this->write(str.data(), str.length());
}
void HttpBodyRaw::write(char const* buf, uint64_t length) STDUTILS_OVERRIDE
{
	this->assertWrite();

	if (!this->knowsWriteableBytes() || this->getWriteableBytes() >= (this->transferred + length))
		this->socket.write(buf, (uint32_t)length);
	else
		throw Exception("Writing more bytes than allowed");
}

bool HttpBodyRaw::knowsReadableBytes() const STDUTILS_OVERRIDE
{
	this->assertRead();

	return this->knowsLength;
}
uint64_t HttpBodyRaw::getReadableBytes() const STDUTILS_OVERRIDE
{
	if (this->knowsReadableBytes())
		return this->contentLength - transferred;
	else
		throw Exception("Body does not know readable bytes");
}
String HttpBodyRaw::read() STDUTILS_OVERRIDE
{
	this->assertRead();

	String rv;

	char buf[4096];
	uint64_t toRead(-1);
	if (this->knowsReadableBytes())
		toRead = this->getReadableBytes();

	//TODO: Aktuell einfach auf 10MB begrenzt. Die Grenze sollte eine Konstante/statische Variable in BodyRaw sein
	//TODO: Keine Fehlerbeldung, wenn die Begrenzung erreicht wird. Dann wird einfach abgeschnitten und zurueckgegeben. Ich brauche eine Exception, die klar macht, dass ein Fehler passiert ist, aber gleichzeitig den gelesenen String enthaelt
	while (toRead > 0 || (toRead == -1 && rv.size() < 10000000 && this->socket.dataAvailable()))
	{
		uint64_t read = this->read(buf, std::min(toRead, (uint64_t)sizeof(buf) - 1));
		buf[read] = 0;

		toRead -= read;
		this->transferred += read;

		rv += buf;
	}
	return rv;
}
uint64_t HttpBodyRaw::read(char* buf, uint64_t bufferSize) STDUTILS_OVERRIDE
{
	uint64_t toRead = bufferSize;
	if (this->knowsReadableBytes())
		toRead = this->getReadableBytes();
	int received = this->socket.read(buf, (uint32_t)std::min(toRead, bufferSize));
	this->transferred += received;

	return received;
}

bool HttpBodyRaw::isFinished() const STDUTILS_OVERRIDE
{
	if (this->isOpen())
	{
		bool knows = false;
		uint64_t bytes;
		if (this->isReadingBody() && this->knowsReadableBytes())
			bytes = this->getReadableBytes();
		else if (this->isWritingBody() && this->knowsWriteableBytes())
			bytes = this->getWriteableBytes();

		if (knows)
			return bytes == 0;
		else
			throw Exception("The body does not know the amount of bytes to be transferred");
	}
	else
		return true;
}
