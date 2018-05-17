#ifndef STDUTILS_DYNAMICLIST_H
#define STDUTILS_DYNAMICLIST_H

#include <StdUtils/Dynamic.h>

#include <vector>

namespace StdUtils
{
	class DynamicList : private std::vector<Dynamic*>
	{
	private:
		typedef std::vector<Dynamic*> listtype;
	public:
		typedef listtype::iterator iterator;
		typedef listtype::const_iterator const_iterator;
		typedef listtype::size_type size_type;

	public:
		explicit DynamicList();
		DynamicList(DynamicList const&);
		~DynamicList();

	public:
		DynamicList& operator=(DynamicList const&);

		bool operator==(DynamicList const&) const;
		bool operator!=(DynamicList const&) const;

		Dynamic& operator[](size_type);
		Dynamic const& operator[](size_type) const;

	private:
		void copy(DynamicList const&);

	public:
		Dynamic const& get(size_type) const;
		Dynamic& get(size_type);
		template<typename T>
		T const& get(size_type i) const
		{
			return this->get(i).getValue<T>();
		}
		template<typename T>
		T& get(size_type i)
		{
			return this->get(i).getValue<T>();
		}

		bool tryGet(size_type, Dynamic&) const;
		template<typename T>
		bool tryGet(size_type i, T& buffer) const
		{
			Dynamic buf;
			if (this->tryGet(i, buf))
			{
				try
				{
					buffer = buf.getValue<T>();
					return true;
				}
				catch (...)
				{
					return false;
				}
			}
			else
				return false;
		}

		void add(Dynamic const&);
		void remove(int);
		void remove(Dynamic const&);

		void clear();

	public:
		using listtype::size;
		using listtype::begin;
		using listtype::end;
	};
}

#endif