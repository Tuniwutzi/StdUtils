#ifndef STDUTILS_QUERY_H
#define STDUTILS_QUERY_H


#include <vector>
#include <map>

#include <StdUtils/Base.h>
#include <StdUtils/Nullable.h>


namespace StdUtils
{
	class Query : private std::map<String, std::vector<Nullable<String> > >
	{
	public:
		typedef std::vector<Nullable<String> > valuelist;
		typedef std::map<String, valuelist> listtype;
		typedef listtype::iterator iterator;
		typedef listtype::const_iterator const_iterator;
		typedef listtype::size_type size_type;

		//private:
		//	listtype arguments;

	public:
		explicit Query();
		explicit Query(String const&);

	private:
		std::pair<String, Nullable<String> > parse(String const&) const;
		void cleanup();

	public:
		Nullable<String>& first(String const&);
		Nullable<String> const& first(String const&) const;

		bool tryFirst(String const&, Nullable<String>&) const;
		Nullable<String> tryFirst(String const&) const;

		valuelist const& get(String const&) const;
		valuelist& get(String const&);
		bool tryGet(String const&, valuelist&) const;

		iterator find(String const&);
		const_iterator find(String const&) const;

		using listtype::begin;
		using listtype::end;
		using listtype::size;
		using listtype::clear;
		using listtype::erase;

		void add(String const&, Nullable<String> const&);
		void add(String const&);

		void set(String const&, Nullable<String> const&);
		void set(String const&);

		//Match name and value
		void remove(String const&, Nullable<String> const&);
		void remove(String const&);

		//Match only the name and behave as the names describe
		void removeFirst(String const&);
		void removeAll(String const&);

		String toString() const;
	};
}

#endif