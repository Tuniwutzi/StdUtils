#ifndef STDUTILS_HTTPHEADERS_H
#define STDUTILS_HTTPHEADERS_H

#include <StdUtils/Networking/Http/HttpHeader.h>
#include <vector>

namespace StdUtils
{
	namespace Networking
	{
		namespace Http
		{
			class HttpHeaders : private std::vector<HttpHeader>
			{
			private:
				typedef std::vector<HttpHeader> listtype;
			public:
				typedef listtype::iterator iterator;
				typedef listtype::const_iterator const_iterator;
				typedef listtype::size_type size_type;

			//private:
			//	listtype headers;

			public:
				explicit HttpHeaders();

			private:
				void assertIndexValid(int index) const;

			public:
				//HttpHeader& get(String const& name);
				//HttpHeader const& get(String const& name) const;
				//HttpHeader& get(int index);
				//HttpHeader const& get(int index) const;
				
				bool tryGet(String const& name, Nullable<String>& buffer) const;
				Nullable<String> const& get(String const& name) const;
				Nullable<String>& get(String const& name);

				const_iterator find(String const& name) const;
				iterator find(String const& name);

				bool has(String const& name) const;

				//bool tryGet(String const& name, HttpHeader& buffer) const;
				//bool tryGet(String const& name, HttpHeader** buffer);

				void set(HttpHeader const& header);
				void set(String const& name, Nullable<String> const& value);
				void set(String const& name, String const& value);
				void set(String const& headerString);

				void add(HttpHeader const& header);
				void add(String const& name, Nullable<String> const& value);
				void add(String const& name, String const& value);
				void add(String const& headerString);

				//void erase(iterator it);
				void erase(String const& name);
				void erase(int index);

				using listtype::at;
				using listtype::clear;
				using listtype::begin;
				using listtype::end;
				using listtype::size;
				using listtype::erase;


				//void clear();

				//iterator begin();
				//const_iterator begin() const;

				//iterator end();
				//const_iterator end() const;

				//int size() const;

				String toString() const;
			};
		}
	}
}

#endif