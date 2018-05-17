#include <StdUtils/DynamicList.h>

#include <StdUtils/DynamicMap.h>
#include <StdUtils/Dynamic.h>
#include <StdUtils/Exceptions.h>


namespace StdUtils
{
	DynamicList::DynamicList()
	{
	}
	DynamicList::DynamicList(DynamicList const& o)
	{
		this->DynamicList::clear();
		this->copy(o);
	}
	DynamicList::~DynamicList()
	{
		this->DynamicList::clear();
	}
	DynamicList& DynamicList::operator=(DynamicList const& o)
	{
		this->DynamicList::clear();
		this->copy(o);
		return *this;
	}
	bool DynamicList::operator==(DynamicList const& o) const
	{
		if (this->size() == o.size())
		{
			for (const_iterator it = this->begin(); it != this->end(); it++)
			{
				bool found = false;
				for (const_iterator it1 = o.begin(); it1 != o.end(); it1++)
				{
					if (**it1 == **it)
					{
						found = true;
						break;
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
	void DynamicList::copy(DynamicList const& o)
	{
		for (const_iterator it = o.begin(); it != o.end(); it++)
			this->add(**it);
	}
	void DynamicList::add(Dynamic const& o)
	{
		this->push_back(new Dynamic(o));
	}
	Dynamic const& DynamicList::get(size_type i) const
	{
		if (this->size() > i && i >= 0)
			return *this->at(i);
		else
			throw Exception("Invalid index");
	}
	Dynamic& DynamicList::get(size_type i)
	{
		if (this->size() > i && i >= 0)
			return *this->at(i);
		else
			throw Exception("Invalid index");
	}

	bool DynamicList::tryGet(size_type i, Dynamic& buffer) const
	{
		try
		{
			buffer = this->get(i);
			return true;
		}
		catch (...)
		{
			return false;
		}
	}

	void DynamicList::clear()
	{
		for (iterator it = this->begin(); it != this->end(); it++)
			delete *it;
		this->listtype::clear();
	}
}