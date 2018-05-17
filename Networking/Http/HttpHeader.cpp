#include <StdUtils/Networking/Http/HttpHeader.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>

using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpHeader::HttpHeader(String const& name, Nullable<String> const& value)
	:name(name),
	value(value)
{
}
HttpHeader::HttpHeader(String const& headerString)
{
	String::size_type idx(headerString.find(':'));
	if (idx != headerString.npos)
	{
		this->name = StringHelpers::Trim(headerString.substr(0, idx));
		this->value = StringHelpers::Trim(headerString.substr(idx + 1));
	}
	else if (headerString.size() > 0)
		this->name = headerString;
	else
		throw Exception("Invalid header string");
}
String const& HttpHeader::getName() const
{
	return this->name;
}
Nullable<String> const& HttpHeader::getValue() const
{
	return this->value;
}
Nullable<String>& HttpHeader::getValue()
{
	return this->value;
}

void HttpHeader::setValue(Nullable<String> const& value)
{
	this->value = value;
}

String HttpHeader::toString() const
{
	if (this->value.hasValue())
		return this->name + ": " + this->value.getValue();
	else
		return this->name;
}