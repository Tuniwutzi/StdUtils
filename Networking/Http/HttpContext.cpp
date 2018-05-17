#include <StdUtils/Networking/Http/HttpContext.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>

#include <private/Networking/Http/HttpBodyRaw.h>
#include <private/Networking/Http/HttpBodyChunked.h>


using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;


HttpContext::HttpContext(Socket<SocketTypes::Tcp>* c, bool ownsSocket)
	:stream(new TcpStream(c, ownsSocket)),
	sstream((SocketStream*)this->stream),
	doesOwnStream(true),
	body(NULL),
	errorState(false)
{
}
HttpContext::HttpContext(NetworkStream* ns, bool ownsStream)
	:stream(ns),
	sstream(dynamic_cast<SocketStream*>(ns)),
	doesOwnStream(ownsStream),
	body(NULL),
	errorState(false)
{
}
HttpContext::~HttpContext()
{
	this->close();
}

void HttpContext::assertWaiting() const
{
	if (this->getState() != HttpContextStates::Waiting)
		throw Exception("HttpClient context is not in waiting state");
}
void HttpContext::assertReceiving() const
{
	if (this->getState() != HttpContextStates::Receiving)
		throw Exception("This context is not in receiving state");
}
void HttpContext::assertSending() const
{
	if (this->getState() != HttpContextStates::Sending)
		throw Exception("This context is not in sending state");
}


void HttpContext::setErrorState()
{
	this->errorState = true;
}


HttpBody* HttpContext::createBody(HttpMessage const& message, bool receiving) const
{
	String transferEncoding;
	try
	{
		transferEncoding = message.getTransferEncoding();
	}
	catch (...)
	{
	}

	if (StringHelpers::Equals(transferEncoding, "chunked", false))
		return new HttpBodyChunked(*this->stream, receiving);
	else
	{
		try
		{
			return new HttpBodyRaw(*this->stream, receiving, message.getContentLength());
		}
		catch (...)
		{
			return new HttpBodyRaw(*this->stream, receiving);
		}
	}
}

HttpContextStates::Value HttpContext::getState() const
{
	if (this->errorState)
		return HttpContextStates::Error;

	if (!this->stream)
		return HttpContextStates::Closed;

	if (this->body && this->body->isOpen())
	{
		if (this->body->isReadingBody())
			return HttpContextStates::Receiving;
		else
			return HttpContextStates::Sending;
	}
	else
		return HttpContextStates::Waiting;
}

NetworkStream* HttpContext::getStream() const
{
	return this->stream;
}
void HttpContext::ownStream()
{
	this->doesOwnStream = true;
}
void HttpContext::disownStream()
{
	this->doesOwnStream = false;
}
bool HttpContext::ownsStream() const
{
	return this->doesOwnStream;
}
void HttpContext::close()
{
	delete this->body;
	this->body = NULL;

	if (this->doesOwnStream)
		delete this->stream;
	this->stream = NULL;
}
