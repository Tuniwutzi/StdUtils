#include <StdUtils/Networking/Http/HttpResponse.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>

using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpResponse::HttpResponse()
	:HttpMessage(MessageTypes::Response),
	statusCode(200),
	statusDescription("OK")
{
}
HttpResponse::~HttpResponse()
{
}

uint16_t HttpResponse::getStatusCode() const
{
	return this->statusCode;
}
void HttpResponse::setStatusCode(uint16_t statusCode)
{
	this->statusCode = statusCode;
}

String HttpResponse::getFirstLine() const
{
	return this->getProtocolVersion() + " " + StringHelpers::ToString(this->getStatusCode()) + " " + this->getStatusDescription();
}
void HttpResponse::setFirstLine(String const& firstLine)
{
	String::size_type offset = 0;

	String::size_type count = firstLine.find(' ', offset);

	if (count == firstLine.npos)
		throw Exception("Invalid header format");


	count -= offset;
	this->setProtocolVersion(firstLine.substr(offset, count));

	offset += count;
	offset++;

	count = firstLine.find(' ', offset);

	if (count == firstLine.npos)
		throw Exception("Invalid header format");


	count -= offset;
	this->statusCode = StringHelpers::Parse<uint16_t>(firstLine.substr(offset, count));

	offset += count;
	offset++;

	this->statusDescription = firstLine.substr(offset);
}

String const& HttpResponse::getStatusDescription() const
{
	return this->statusDescription;
}
void HttpResponse::setStatusDescription(String const& statusDescription)
{
	this->statusDescription = statusDescription;
}

static struct {
	KnownStatusCodes::Value SC;
	char Description[256];
} knownStatusCodes[] =
{
	{ KnownStatusCodes::OK, "OK" },
	{ KnownStatusCodes::BadRequest, "BAD REQUEST" },
	{ KnownStatusCodes::NotFound, "NOT FOUND" },
	{ KnownStatusCodes::UpgradeRequired, "UPGRADE REQUIRED" },
	{ KnownStatusCodes::ServerError, "SERVER ERROR" }
};
KnownStatusCodes::Value HttpResponse::getKnownStatusCode() const
{
	return (KnownStatusCodes::Value)this->statusCode;
}
void HttpResponse::setKnownStatusCode(KnownStatusCodes::Value code, bool updateDescription)
{
	this->statusCode = code;
	if (updateDescription)
	{
		int size = sizeof(knownStatusCodes) / sizeof(*knownStatusCodes);
		for (int i = 0; i < size; i++)
		{
			if (this->statusCode == knownStatusCodes[i].SC)
			{
				this->statusDescription = knownStatusCodes[i].Description;
				break;
			}
		}
		//TODO: Was, wenn zum Code keine Description gefunden wurde?
	}
}

String HttpResponse::toString() const
{
	return this->getFirstLine() + "\r\n" + this->getHeaders().toString();
}