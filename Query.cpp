#include <StdUtils/Uri.h>

using namespace std;

namespace StdUtils
{
	Query::Query()
	{
	}
	Query::Query(String const& queryString)
	{
		String::size_type offset = 0;

		if (queryString.find("?") == 0)
			offset++;

		String::size_type argIndex;
		while ((argIndex = queryString.find("&", offset)) != queryString.npos)
		{
			if (argIndex == offset)
				offset++;
			else
			{
				String argument = queryString.substr(offset, argIndex - offset);
				offset=argIndex + 1;

				this->add(argument);
			}
		}
		String arg = queryString.substr(offset);
		if (arg.size() > 0)
			this->add(arg);
	}

	pair<String, Nullable<String> > Query::parse(String const& argument) const
	{
		String::size_type eqIdx = argument.find("=");
		if (eqIdx == argument.npos)
			return pair<String, Nullable<String> >(argument, Nullable<String>());
		else
			return pair<String, Nullable<String> >(argument.substr(0, eqIdx), argument.substr(eqIdx + 1));
	}
	void Query::cleanup()
	{
		for (iterator it = this->begin(); it != this->end();)
		{
			if (it->second.size() == 0)
				this->erase(it++); //g++ implementiert erase als: void erase(iterator). erase(it++) macht: it um 1 erhoehen, alten it zurueckgeben, dieser wird geloescht
			else
				it++;
		}
	}

	Nullable<String> const& Query::first(String const& name) const
	{
		const_iterator it = this->find(name);
		if (it == this->end() || it->second.size() == 0)
			throw Exception(String("No query argument with name ") + name + " found");

		return *it->second.begin();
	}
	Nullable<String>& Query::first(String const& name)
	{
		iterator it = this->find(name);
		if (it == this->end() || it->second.size() == 0)
			throw Exception(String("No query argument with name ") + name + " found");

		return *it->second.begin();
	}

	bool Query::tryFirst(String const& name, Nullable<String>& buffer) const
	{
		const_iterator it = this->find(name);
		if (it != this->end() && it->second.size() > 0)
		{
			buffer = *it->second.begin();
			return true;
		}
		else
			return false;
	}
	Nullable<String> Query::tryFirst(String const& name) const
	{
		Nullable<String> nb;
		this->tryFirst(name, nb);
		return nb;
	}

	Query::valuelist& Query::get(String const& name)
	{
		iterator it = this->find(name);
		if (it == this->end() || it->second.size() == 0)
			throw Exception(String("No query argument with name ") + name + " found");

		return it->second;
	}
	Query::valuelist const& Query::get(String const& name) const
	{
		const_iterator it = this->find(name);
		if (it == this->end() || it->second.size() == 0)
			throw Exception(String("No query argument with name ") + name + " found");

		return it->second;
	}

	bool Query::tryGet(String const& name, Query::valuelist& buffer) const
	{
		const_iterator it = this->find(name);
		if (it != this->end())
		{
			buffer = it->second;
			return true;
		}
		else
			return false;
	}

	Query::iterator Query::find(String const& name)
	{
		for (iterator it = this->begin(); it != this->end(); it++)
		{
			if (it->first == name)
				return it;
		}
		return this->end();
	}
	Query::const_iterator Query::find(String const& name) const
	{
		for (const_iterator it = this->begin(); it != this->end(); it++)
		{
			if (it->first == name)
				return it;
		}
		return this->end();
	}

	void Query::add(String const& name, Nullable<String> const& value)
	{
		this->operator[](name).push_back(value);
	}
	void Query::add(String const& argument)
	{
		pair<String, Nullable<String> > parsed(this->parse(argument));
		this->add(parsed.first, parsed.second);
	}

	void Query::set(String const& name, Nullable<String> const& value)
	{
		iterator it = this->find(name);
		if (it != this->end())
			it->second.push_back(value);
		else
			this->add(name, value);
	}
	void Query::set(String const& argument)
	{
		pair<String, Nullable<String> > parsed(this->parse(argument));
		this->set(parsed.first, parsed.second);
	}

	void Query::remove(String const& name, Nullable<String> const& value)
	{
		iterator it = this->find(name);
		if (it != this->end())
		{
			for (valuelist::iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
			{
				if ((!it1->hasValue() && !value.hasValue()) || (it1->hasValue() == value.hasValue() && it1->getValue() == value.getValue()))
				{
					it->second.erase(it1);
					this->cleanup();
					break;
				}
			}
		}
	}
	void Query::remove(String const& argument)
	{
		pair<String, Nullable<String> > parsed(this->parse(argument));
		this->remove(parsed.first, parsed.second);
	}

	void Query::removeFirst(String const& name)
	{
		iterator it = this->find(name);
		if (it != this->end() && it->second.size() > 0)
			it->second.erase(it->second.begin());
		this->cleanup();
	}
	void Query::removeAll(String const& name)
	{
		iterator it = this->find(name);
		if (it != this->end())
			this->erase(it);
	}

	String Query::toString() const
	{
		String rv = "?";
		for (const_iterator it = this->begin(); it != this->end(); it++)
		{
			for (valuelist::const_iterator it1 = it->second.begin(); it1 != it->second.end(); it1++)
			{
				if (!rv.size() == 1)
					rv += "&";

				rv += it->first;
				if (it1->hasValue())
					rv += "=" + it1->getValue();
			}
		}
		return rv;
	}
}