#include <StdUtils/Networking/Http/HttpRequest.h>

#include <StdUtils/Exceptions.h>

using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpRequest::HttpRequest()
	:HttpMessage(MessageTypes::Request),
	 ressource("http", "/")
{
}
HttpRequest::~HttpRequest()
{
}


String const& HttpRequest::getMethod() const
{
	return this->method;
}
void HttpRequest::setMethod(String const& method)
{
	this->method = method;
}

Uri<UriFormats::Http> const& HttpRequest::getRessource() const
{
	return this->ressource;
}
Uri<UriFormats::Http>& HttpRequest::getRessource()
{
	return this->ressource;
}
void HttpRequest::setRessource(Uri<UriFormats::Http> const& ressource)
{
	this->ressource = ressource;
	if (!this->ressource.isRelative())
	{
		this->getHeaders().set("host", this->ressource.getHost().getValue());
		this->ressource.setHost(Nullable<String>());
	}
	if (this->ressource.getPath().find("/") != 0)
		this->ressource.setPath("/" + this->ressource.getPath());
}
void HttpRequest::setRessource(String const& ressource)
{
	if (ressource.find("/") != 0)
		this->ressource = Uri<UriFormats::Http>("http", "/" + ressource);
	else
		this->ressource = Uri<UriFormats::Http>("http", ressource);
}

String HttpRequest::getFirstLine() const
{
	return this->getMethod() + " " + this->getRessource().toRelativeString(false) + " " + this->getProtocolVersion();
}
void HttpRequest::setFirstLine(String const& firstLine)
{
	String::size_type offset = 0;
	String::size_type index = firstLine.find(' ', offset);

	if (index == firstLine.npos)
		throw Exception("Invalid header format");
	else
		this->method = firstLine.substr(offset, index - offset);

	offset = index + 1;

	index = firstLine.find(' ', offset);
	if (index == firstLine.npos)
		throw Exception("Invalid header format");
	else
		this->ressource = Uri<UriFormats::Http>("http", firstLine.substr(offset, index - offset));

	this->setProtocolVersion(firstLine.substr(index + 1));
}

static struct {
	char Name[512];
	KnownMethods::Value Value;
} knownMethods[] =
{
	{ "GET", KnownMethods::GET },
	{ "POST", KnownMethods::POST }
};
KnownMethods::Value HttpRequest::getKnownMethod() const
{
	int count = sizeof(knownMethods) / sizeof(*knownMethods);
	for (int i = 0; i < count; i++)
	{
		if (this->method == knownMethods[i].Name)
			return knownMethods[i].Value;
	}

	throw Exception("HttpRequest has an unknown method");
}

String const& HttpRequest::getHost() const
{
	return this->getHeaders().get("host").getValue();
}

Uri<UriFormats::Http> HttpRequest::getRequestUri() const
{
	if (this->getHeaders().has("host"))
	{
		Uri<UriFormats::Http> uri(this->ressource);
		uri.setHost(this->getHost());
		return uri;
	}
	else
		return this->getRessource();
}

String HttpRequest::toString() const
{
	return this->getFirstLine() + "\r\n" + this->getHeaders().toString();
}
