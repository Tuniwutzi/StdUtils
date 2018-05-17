#include <StdUtils/Networking/Http/HttpHeaders.h>

#include <StdUtils/Exceptions.h>
#include <StdUtils/StringHelpers.h>

using namespace StdUtils;
using namespace StdUtils::Networking;
using namespace StdUtils::Networking::Http;

HttpHeaders::HttpHeaders()
{
}

void HttpHeaders::assertIndexValid(int index) const
{
	if (index < 0)
		throw Exception("Index must be at least 0");
	if (index >= this->size())
		throw Exception("Index must be smaller than header count");
}

bool HttpHeaders::tryGet(String const& name, Nullable<String>& buffer) const
{
	const_iterator it = this->find(name);
	if (it != this->end())
	{
		buffer = it->getValue();
		return true;
	}
	else
		return false;
}
Nullable<String> const& HttpHeaders::get(String const& name) const
{
	const_iterator it = this->find(name);
	if (it == this->end())
		throw Exception(String("Header ") + name + " not found");

	return it->getValue();
}
Nullable<String>& HttpHeaders::get(String const& name)
{
	iterator it = this->find(name);
	if (it == this->end())
		throw Exception(String("Header ") + name + " not found");

	return it->getValue();
}


HttpHeaders::const_iterator HttpHeaders::find(String const& name) const
{
	for (const_iterator it = this->begin(); it != this->end(); it++)
	{
		if (StringHelpers::Equals(it->getName(), name, false))
			return it;
	}

	return this->end();
}
HttpHeaders::iterator HttpHeaders::find(String const& name)
{
	for (iterator it = this->begin(); it != this->end(); it++)
	{
		if (StringHelpers::Equals(it->getName(), name, false))
			return it;
	}

	return this->end();
}

bool HttpHeaders::has(String const& name) const
{
	return this->find(name) != this->end();
}

//HttpHeader& HttpHeaders::get(String const& name)
//{
//	iterator it(this->findHeader(name));
//	if (it != this->end())
//		return *it;
//	else
//		throw Exception("Header not found");
//}
//HttpHeader const&  HttpHeaders::get(String const& name) const
//{
//	const_iterator it(this->findHeader(name));
//	if (it != this->end())
//		return *it;
//	else
//		throw Exception("Header not found");
//}
//HttpHeader& HttpHeaders::get(int index)
//{
//	this->assertIndexValid(index);
//
//	return *(this->begin() + index);
//}
//HttpHeader const& HttpHeaders::get(int index) const
//{
//	this->assertIndexValid(index);
//
//	return *(this->begin() + index);
//}
//
//bool HttpHeaders::tryGet(String const& name, HttpHeader& buffer) const
//{
//	const_iterator it(this->findHeader(name));
//
//	if (it != this->end())
//	{
//		buffer = *it;
//		return true;
//	}
//	else
//		return false;
//}
//bool HttpHeaders::tryGet(String const& name, HttpHeader** buffer)
//{
//	iterator it(this->findHeader(name));
//
//	if (it != this->end())
//	{
//		*buffer = &*it;
//		return true;
//	}
//	else
//		return false;
//}

void HttpHeaders::add(HttpHeader const& header)
{
	this->push_back(header);
}
void HttpHeaders::add(String const& name, Nullable<String> const& value)
{
	this->add(HttpHeader(name, value));
}
void HttpHeaders::add(String const& name, String const& value)
{
	this->add(HttpHeader(name, value));
}
void HttpHeaders::add(String const& headerString)
{
	this->add(HttpHeader(headerString));
}

void HttpHeaders::set(HttpHeader const& header)
{
	iterator it(this->find(header.getName()));
	if (it != this->end())
		it->setValue(header.getValue());
	else
		this->add(header.getName(), header.getValue());
}
void HttpHeaders::set(String const& name, Nullable<String> const& value)
{
	this->set(HttpHeader(name, value));
}
void HttpHeaders::set(String const& name, String const& value)
{
	this->set(HttpHeader(name, value));
}
void HttpHeaders::set(String const& headerString)
{
	this->set(HttpHeader(headerString));
}

//void HttpHeaders::erase(iterator it)
//{
//	this->headers.erase(it);
//}
void HttpHeaders::erase(int index)
{
	this->assertIndexValid(index);

	this->erase(this->begin() + index);
}
void HttpHeaders::erase(String const& name)
{
	iterator it(this->find(name));
	if (it != this->end())
		this->erase(it);
}

//void HttpHeaders::clear()
//{
//	this->headers.clear();
//}
//
//HttpHeaders::iterator HttpHeaders::begin()
//{
//	return this->headers.begin();
//}
//HttpHeaders::const_iterator HttpHeaders::begin() const
//{
//	return this->headers.begin();
//}
//
//HttpHeaders::iterator HttpHeaders::end()
//{
//	return this->headers.end();
//}
//HttpHeaders::const_iterator HttpHeaders::end() const
//{
//	return this->headers.end();
//}
//
//int HttpHeaders::size() const
//{
//	return this->headers.size();
//}

String HttpHeaders::toString() const
{
	String rv;
	for (const_iterator it = this->begin(); it != this->end(); it++)
		rv += it->toString() + "\r\n";
	return rv + "\r\n";
}