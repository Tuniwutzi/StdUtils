#include <StdUtils/Networking/Http/HttpMessage.h>


#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>


using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpMessage::HttpMessage(MessageTypes::Value type)
	:type(type),
	protocolVersion("HTTP/1.1")
{
}

HttpHeaders& HttpMessage::getHeaders()
{
	return this->headers;
}
HttpHeaders const& HttpMessage::getHeaders() const
{
	return this->headers;
}
void HttpMessage::setHeaders(HttpHeaders const& headers)
{
	this->headers = headers;
}

String const& HttpMessage::getProtocolVersion() const
{
	return this->protocolVersion;
}
void HttpMessage::setProtocolVersion(String const& protocolVersion)
{
	this->protocolVersion = protocolVersion;
}

void HttpMessage::setContentType(String const& contentType)
{
	this->headers.set("content-type", contentType);
}
String const& HttpMessage::getContentType() const
{
	return this->headers.get("content-type").getValue();
}

void HttpMessage::setContentLength(unsigned int length)
{
	this->headers.set("content-length", StringHelpers::ToString(length));
}
void HttpMessage::removeContentLength()
{
	this->headers.erase("content-length");
}
unsigned int HttpMessage::getContentLength() const
{
	return StringHelpers::Parse<unsigned int>(this->headers.get("content-length").getValue());
}

void HttpMessage::setTransferEncoding(String const& transferEncoding)
{
	this->headers.set("transfer-encoding", transferEncoding);
}
String const& HttpMessage::getTransferEncoding() const
{
	return this->headers.get("transfer-encoding").getValue();
}