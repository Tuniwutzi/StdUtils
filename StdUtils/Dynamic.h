#ifndef STDUTILS_DYNAMIC_H
#define STDUTILS_DYNAMIC_H

#include <StdUtils/Nullable.h>

#include <sstream>

namespace StdUtils
{
	/*
	Typ hinzufuegen:
		-In Enum angeben 
		-ResolveType-Spezialisierung erstellen
		-Case-Label in apply erstellen
		-Wenn numerischer Typ: Case-Label in boxNumber erstellen
	*/
	STDUTILS_START_ENUM(DynamicTypes)
	{
		Null	= 0x00000000,
			Char	= 0x00100001,
			UChar	= 0x10100002,
			Int16	= 0x00100003,
			UInt16	= 0x10100004,
			Int32	= 0x00100005,
			UInt32	= 0x10100006,
			Int64	= 0x00100007,
			UInt64	= 0x10100008,
			Float	= 0x01100009,
			Double	= 0x01100010,
			LDouble	= 0x01100011,
			Boolean	= 0x00000012,
			String	= 0x00000013,
			Map		= 0x00000014,
			List	= 0x00000015,
			Pointer	= 0x00000016,
			Date	= 0x00000017,
			Time	= 0x00000018,
			DateTime= 0x00000019
	}
	STDUTILS_END_ENUM(DynamicTypes);

	class Dynamic
	{
	private:
		void* value;
		DynamicTypes::Value type;

	public:
		Dynamic();
		template<typename T>
		Dynamic(T const& val)
			:value(new T(val)),
			type(Dynamic::ResolveType<T>())
		{
		}
		Dynamic(char const*); //Not explicit, because that would result in the templated constructor being implicitly called and causing a nonsensical compiler error

		template<typename T>
		Dynamic(Nullable<T> const& val)
			:value(NULL),
			type(DynamicTypes::Null)
		{
			if (val.hasValue())
				this->operator=(Dynamic(val.getValue()));
		}

		Dynamic(Dynamic const&);

		~Dynamic();

	public:
		Dynamic& operator=(Dynamic const&);

		bool operator==(Dynamic const&) const;
		bool operator!=(Dynamic const&) const;

	private:
		template<typename T>
		bool doApply(Dynamic const& instance, Dynamic const* compareTo, void** copyTo, bool destroy) const
		{
			bool rv = false;

			if (compareTo && compareTo->getType() == instance.getType())
				rv = instance.isNull() || *(T*)instance.value == *(T*)compareTo->value;

			if (copyTo)
				*copyTo = instance.isNull() ? NULL : new T(*(T*)instance.value);

			if (destroy && !instance.isNull())
				delete (T*)instance.value;

			return rv;
		}
		bool apply(Dynamic const& instance, Dynamic const* compareTo, void** copyTo, bool destroy) const;

		void copy(Dynamic const&);
		void destroy();
		bool compare(int, ...) const;

		void assertType(DynamicTypes::Value) const;
		template<typename T>
		void assertType() const
		{
			if (Dynamic::ResolveType<T>() != this->getType())
				throw Exception("Type mismatch");
		}

	public:
		template<typename T>
		T const& getValue() const
		{
			this->assertType<T>();
			return *(T*)this->value;
		}
		template<typename T>
		T& getValue()
		{
			this->assertType<T>();
			return *(T*)this->value;
		}

		template<typename T>
		bool ofType() const
		{
			return this->getType() == Dynamic::ResolveType<T>();
		}

		//these functions do some conversions if the types dont match exactly
		fpmax asDouble() const;
		intmax_t asInteger(bool acceptPrecisionLoss = false) const;
		uintmax_t asUnsignedInteger(bool acceptPrecisionLoss = false) const;

		DynamicTypes::Value getType() const;
		bool isNull() const;
		bool isNumeric() const;
		bool isSigned() const;
		bool isFloatingPoint() const;

	public:
		template<typename T>
		static DynamicTypes::Value ResolveType();
	};
}

#endif
