#include <StdUtils/Networking/Http/HttpServerContext.h>

#include <StdUtils/Exceptions.h>

#include <private/Networking/Http/HttpReader.h>

using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpServerContext::HttpServerContext(Socket<SocketTypes::Tcp>* c,
        bool ownsSocket)
		: HttpContext(c, ownsSocket)
{
}
HttpServerContext::HttpServerContext(NetworkStream* s, bool ownsStream)
		: HttpContext(s, ownsStream)
{
}
HttpServerContext::~HttpServerContext()
{
}

bool HttpServerContext::requestReceived() const
{
	return this->getState() != HttpContextStates::Waiting;
}
void HttpServerContext::receiveRequest()
{
	try
	{
		HttpReader::ReadMessage(*this->stream, this->request);

		this->body = this->createBody(this->request, true);
	}
	catch (...)
	{
		this->setErrorState();
		throw;
	}
}

IHttpBodyReader& HttpServerContext::getRequestBody()
{
	this->assertReceiving();

	return *this->body;
}
IHttpBodyWriter& HttpServerContext::sendResponse()
{
	this->assertReceiving();

	delete this->body;
	this->body = NULL;

	String rsString(this->response.toString());
	this->stream->write(rsString.data(), (uint32_t)rsString.size());
	this->body = this->createBody(this->response, false);

	return *this->body;
}
bool HttpServerContext::responseSent() const
{
	//TODO!: Nicht so ganz richtig, aber scheint es keine Moeglichkeit fuer zu geben; Vermutlich die Kontexte insgesamt nochmal ueberarbeiten
	return this->getState() == HttpContextStates::Sending
	        || this->getState() == HttpContextStates::Closed;
}

HttpRequest const& HttpServerContext::getRequest() const
{
	if (this->getState() == HttpContextStates::Waiting)
		throw Exception("HttpContext has not received the request yet");

	return this->request;
}
HttpResponse const& HttpServerContext::getResponse() const
{
	return this->response;
}
HttpResponse& HttpServerContext::getResponse()
{
	//TODO?: Wie verhindern, dass die HttpResponse veraendert werden kann, nachdem sie schon abgeschickt wurde?
	return this->response;
}
void HttpServerContext::setResponse(HttpResponse const& response)
{
	this->assertReceiving();

	this->response = response;
}
