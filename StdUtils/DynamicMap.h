#ifndef STDUTILS_DYNAMICMAP_H
#define STDUTILS_DYNAMICMAP_H

#include <StdUtils/Dynamic.h>
#include <StdUtils/Persistance/IPersistantMap.h>

#include <map>
#include <vector>

namespace StdUtils
{
	class DynamicMap : public Persistance::IPersistantMap, private std::map<String, Dynamic*>
	{
	private:
		typedef std::map<String, Dynamic*> listtype;
	public:
		typedef listtype::iterator iterator;
		typedef listtype::const_iterator const_iterator;
		typedef listtype::size_type size_type;

	public:
		explicit DynamicMap();
		DynamicMap(DynamicMap const&);
		~DynamicMap();

	public:
		DynamicMap& operator=(DynamicMap const&);

		bool operator==(DynamicMap const&) const;
		bool operator!=(DynamicMap const&) const;

		//No const overload, because it automatically inserts a key if it is not present
		Dynamic& operator[](String const&);

	private:
		void copy(DynamicMap const&);
		virtual Dynamic resolvePersistantValue(String const& fieldName, DynamicTypes::Value valueType) const STDUTILS_OVERRIDE;

	public:
		Dynamic const& get(String const& field) const;
		Dynamic& get(String const& field);
		template<typename T>
		T const& get(String const& field) const
		{
			return this->get(field).getValue<T>();
		}
		template<typename T>
		T& get(String const& field)
		{
			return this->get(field).getValue<T>();
		}

		bool tryGet(String const&, Dynamic& buffer) const;
		template<typename T>
		bool tryGet(String const& field, T& buffer) const
		{
			Dynamic buf;
			if (!this->tryGet(field, buf))
				return false;
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

		virtual bool isNull(String const& fieldName) const STDUTILS_OVERRIDE;


		void set(String const& field, Dynamic const& value);
		void setNull(String const& field);

		void clear();

	public:
		using listtype::size;
		using listtype::begin;
		using listtype::end;
		using listtype::find;
	};
}

#endif