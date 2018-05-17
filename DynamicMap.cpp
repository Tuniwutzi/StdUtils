#include <StdUtils/DynamicMap.h>


#include <StdUtils/DynamicList.h>
#include <StdUtils/Dynamic.h>
#include <StdUtils/Exceptions.h>


namespace StdUtils
{
	DynamicMap::DynamicMap()
	{
	}
	DynamicMap::DynamicMap(DynamicMap const& o)
	{
		this->DynamicMap::clear();
		this->copy(o);
	}
	DynamicMap::~DynamicMap()
	{
		this->DynamicMap::clear();
	}
	DynamicMap& DynamicMap::operator=(DynamicMap const& o)
	{
		this->DynamicMap::clear();
		this->copy(o);
		return *this;
	}
	bool DynamicMap::operator==(DynamicMap const& o) const
	{
		if (this->size() == o.size())
		{
			for (const_iterator it = this->begin(); it != this->end(); it++)
			{
				bool found = false;
				for (const_iterator it1 = o.begin(); it1 != o.end(); it1++)
				{
					if (it1->first == it->first)
					{
						if (*it1->second == *it->second)
						{
							found = true;
							break;
						}
					}
				}
				if (!found)
					return false;
			}

			return true;
		}
		else
			return false;
	}
	Dynamic& DynamicMap::operator[](String const& field)
	{
		Dynamic*& rv = map<String, Dynamic*>::operator[](field);

		if (!rv)
			rv = new Dynamic;

		return *rv;
	}


	void DynamicMap::copy(DynamicMap const& o)
	{
		for (const_iterator it = o.begin(); it != o.end(); it++)
			this->set(it->first, *it->second);
	}
	Dynamic DynamicMap::resolvePersistantValue(String const& fieldName, DynamicTypes::Value fieldType) const
	{
		Dynamic rv(this->get(fieldName));
		if (rv.getType() == fieldType)
			return rv;
		else
			throw Exception("Type of persistant value is not as expected");
	}


	void DynamicMap::clear()
	{
		for (iterator it = this->begin(); it != this->end(); it++)
			delete it->second;
		this->listtype::clear();
	}
	Dynamic const& DynamicMap::get(String const& field) const
	{
		return *this->at(field);
	}
	Dynamic& DynamicMap::get(String const& field)
	{
		return *this->at(field);
	}

	bool DynamicMap::tryGet(String const& field, Dynamic& buffer) const
	{
		try
		{
			buffer = this->get(field);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	bool DynamicMap::isNull(String const& fieldName) const STDUTILS_OVERRIDE
	{
		return this->get(fieldName).isNull();
	}

	void DynamicMap::set(String const& field, Dynamic const& value)
	{
		this->listtype::operator[](field) = new Dynamic(value);
	}
	void DynamicMap::setNull(String const& field)
	{
		this->erase(field);
	}
}