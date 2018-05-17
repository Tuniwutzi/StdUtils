#include <StdUtils/Networking/Http/HttpClientContext.h>


#include <StdUtils/Exceptions.h>

#include <private/Networking/Http/HttpReader.h>


using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;


HttpClientContext::HttpClientContext(Socket<SocketTypes::Tcp>* c, bool ownsSocket)
	:HttpContext(c, ownsSocket)
{
}
HttpClientContext::HttpClientContext(NetworkStream* stream, bool ownsStream)
	:HttpContext(stream, ownsStream)
{
}
HttpClientContext::~HttpClientContext()
{
}


IHttpBodyReader& HttpClientContext::getResponseBody()
{
	this->assertReceiving();

	return *this->body;
}
IHttpBodyWriter& HttpClientContext::sendRequest()
{
	this->assertWaiting();

	String rqString(this->request.toString());
	this->stream->write(rqString.data(), (uint32_t)rqString.size());
	this->body = this->createBody(this->request, false);

	return *this->body;
}

HttpRequest& HttpClientContext::getRequest()
{
	//TODO?: Wie verhindern, dass der HttpRequest veraendert werden kann, nachdem er schon abgeschickt wurde?
	return this->request;
}
void HttpClientContext::setRequest(HttpRequest const& request)
{
	this->assertWaiting();

	this->request = request;
}

bool HttpClientContext::responseReceived() const
{
	HttpContextStates::Value st(this->getState());
	return st == HttpContextStates::Closed || st == HttpContextStates::Receiving;
}
HttpResponse const& HttpClientContext::receiveResponse()
{
	if (this->getState() == HttpContextStates::Sending)
	{
		try
		{
			HttpReader::ReadMessage(*this->stream, this->response);

			delete this->body;
			this->body = this->createBody(this->response, true);
		}
		catch (...)
		{
			this->setErrorState();
			throw;
		}
	}
	else
		throw Exception("Invalid context state");

	return this->response;
}
HttpResponse const& HttpClientContext::getResponse() const
{
	if (this->getState() == HttpContextStates::Receiving || this->getState() == HttpContextStates::Closed)
		return this->response;
	else
		throw Exception("Invalid context state");
}