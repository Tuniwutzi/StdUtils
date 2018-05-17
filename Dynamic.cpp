#include <StdUtils/Dynamic.h>


#include <StdUtils/DynamicList.h>
#include <StdUtils/DynamicMap.h>

#include <StdUtils/DateTime.h>

#include <StdUtils/Exceptions.h>

#include <math.h> //modf in linux


namespace StdUtils
{
	Dynamic::Dynamic()
		:value(NULL),
		type(DynamicTypes::Null)
	{
	}

	Dynamic::Dynamic(char const* val)
		:value(new String(val)),
		type(DynamicTypes::String)
	{
	}

	Dynamic::Dynamic(Dynamic const& o)
		:value(NULL),
		type(DynamicTypes::Null)
	{
		this->copy(o);
	}
	Dynamic::~Dynamic()
	{
		this->destroy();
	}

	Dynamic& Dynamic::operator=(Dynamic const& o)
	{
		this->destroy();
		this->copy(o);
		return *this;
	}


	bool Dynamic::apply(Dynamic const& instance, Dynamic const* compareTo, void** copyTo, bool destroy) const
	{
		switch (instance.getType())
		{
		case DynamicTypes::Null:
			{
				if (copyTo)
					*copyTo = NULL;
				if (compareTo)
					return compareTo->isNull();
				else
					return false;
			}
			break;
		case DynamicTypes::Char:
			return doApply<char>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::UChar:
			return doApply<unsigned char>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Int16:
			return doApply<int16_t>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::UInt16:
			return doApply<uint16_t>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Int32:
			return doApply<int32_t>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::UInt32:
			return doApply<uint32_t>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Int64:
			return doApply<int64_t>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::UInt64:
			return doApply<uint64_t>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Float:
			return doApply<float>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Double:
			return doApply<double>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::LDouble:
			return doApply<long double>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Boolean:
			return doApply<bool>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::String:
			return doApply<String>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::List:
			return doApply<DynamicList>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Map:
			return doApply<DynamicMap>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Pointer:
			return doApply<void*>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Date:
			return doApply<Date>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::Time:
			return doApply<Time>(instance, compareTo, copyTo, destroy);
			break;
		case DynamicTypes::DateTime:
			return doApply<DateTime>(instance, compareTo, copyTo, destroy);
			break;
		}

		throw Exception("Unknown type");
	}

	bool Dynamic::operator==(Dynamic const& o) const
	{
		return apply(*this, &o, NULL, false);
	}
	bool Dynamic::operator!=(Dynamic const& o) const
	{
		return !((*this) == o);
	}

	void Dynamic::destroy()
	{
		apply(*this, NULL, NULL, true);
	}
	void Dynamic::copy(Dynamic const& o)
	{
		this->type = o.getType();
		apply(o, NULL, &this->value, false);
	}
	void Dynamic::assertType(DynamicTypes::Value type) const
	{
		if (this->getType() != type)
			throw Exception("Invalid type");
	}

	struct NumberBox
	{
		bool Signed;
		bool FloatingPoint;
		union
		{
			uintmax_t UInt;
			intmax_t Int;
			fpmax FloatingPoint;
		} Values;
	};
	static NumberBox boxNumber(Dynamic const& dyn)
	{
		NumberBox rv;
		rv.Signed = dyn.isSigned();
		rv.FloatingPoint = false;

		switch (dyn.getType())
		{
		case DynamicTypes::Double:
			rv.FloatingPoint = true;
			rv.Values.FloatingPoint = dyn.getValue<double>();
			break;
		case DynamicTypes::LDouble:
			rv.FloatingPoint = true;
			rv.Values.FloatingPoint = dyn.getValue<long double>();
			break;
		case DynamicTypes::Float:
			rv.FloatingPoint = true;
			rv.Values.FloatingPoint = dyn.getValue<float>();
			break;
		case DynamicTypes::Char:
			rv.Values.Int = dyn.getValue<char>();
			break;
		case DynamicTypes::UChar:
			rv.Values.UInt = dyn.getValue<unsigned char>();
			break;
		case DynamicTypes::Int16:
			rv.Values.Int = dyn.getValue<int16_t>();
			break;
		case DynamicTypes::UInt16:
			rv.Values.UInt = dyn.getValue<uint16_t>();
			break;
		case DynamicTypes::Int32:
			rv.Values.Int = dyn.getValue<int32_t>();
			break;
		case DynamicTypes::UInt32:
			rv.Values.UInt = dyn.getValue<uint32_t>();
			break;
		case DynamicTypes::Int64:
			rv.Values.Int = dyn.getValue<int64_t>();
			break;
		case DynamicTypes::UInt64:
			rv.Values.UInt = dyn.getValue<uint64_t>();
			break;
		default:
			throw Exception("Can not convert to Double");
			break;
		}

		//TODO: Folgendes mal testen und eventuell drinlassen
//		if (rv.FloatingPoint)
//		{
//			if (floor(rv.Values.FloatingPoint) == rv.Values.FloatingPoint)
//			{
//				rv.Values.Int = rv.Values.FloatingPoint;
//				rv.FloatingPoint = false;
//			}
//		}

		return rv;
	}
	fpmax Dynamic::asDouble() const
	{
		NumberBox nb = boxNumber(*this);

		if (nb.FloatingPoint)
			return nb.Values.FloatingPoint;
		else
		{
			fpmax rv;
			bool lostPrecision;
			if (nb.Signed)
			{
				rv = (fpmax)nb.Values.Int;
				lostPrecision = rv != nb.Values.Int;
			}
			else
			{
				rv = (fpmax)nb.Values.UInt;
				lostPrecision = rv != nb.Values.UInt;
			}

			if (lostPrecision)
				throw Exception("Value too big for double");

			return rv;
		}
	}
	intmax_t Dynamic::asInteger(bool acceptPrecisionLoss) const
	{
		NumberBox nb(boxNumber(*this));

		if (nb.FloatingPoint)
		{
			double intpart;
			if (!acceptPrecisionLoss && modf(nb.FloatingPoint, &intpart) != 0)
				throw Exception("Loosing precision");

			return (intmax_t)nb.Values.FloatingPoint;
		}
		else
		{
			if (nb.Signed)
				return nb.Values.Int;
			else if (((intmax_t)nb.Values.UInt) >= 0L)
				return (intmax_t)nb.Values.UInt;
			else
				throw Exception("Value too big for signed int64");
		}
	}
	uintmax_t Dynamic::asUnsignedInteger(bool acceptPrecisionLoss) const
	{
		NumberBox nb(boxNumber(*this));

		if (nb.FloatingPoint)
		{
			if (!acceptPrecisionLoss)
				throw Exception("Loosing precision");

			return (uintmax_t)nb.Values.FloatingPoint;
		}
		else
		{
			if (nb.Signed)
				return (uintmax_t)nb.Values.Int;
			else
				return nb.Values.UInt;
		}
	}

	DynamicTypes::Value Dynamic::getType() const
	{
		return this->type;
	}

	bool Dynamic::isNull() const
	{
		return this->type == DynamicTypes::Null;
	}

	bool Dynamic::isNumeric() const
	{
		return (this->type & 0x00100000) != 0;
	}
	bool Dynamic::isSigned() const
	{
		return this->isNumeric() && (this->type & 0x10000000) == 0;
	}
	bool Dynamic::isFloatingPoint() const
	{
		return this->isNumeric() && (this->type & 0x01000000) != 0;
	}

	

	
	template<>
	DynamicTypes::Value Dynamic::ResolveType<char>()
	{
		return DynamicTypes::Char;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<unsigned char>() 
	{
		return DynamicTypes::UChar;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<int16_t>() 
	{
		return DynamicTypes::Int16;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<uint16_t>() 
	{
		return DynamicTypes::UInt16;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<int32_t>() 
	{
		return DynamicTypes::Int32;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<uint32_t>() 
	{
		return DynamicTypes::UInt32;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<int64_t>() 
	{
		return DynamicTypes::Int64;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<uint64_t>() 
	{
		return DynamicTypes::UInt64;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<float>() 
	{
		return DynamicTypes::Float;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<double>() 
	{
		return DynamicTypes::Double;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<long double>() 
	{
		return DynamicTypes::LDouble;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<bool>() 
	{
		return DynamicTypes::Boolean;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<String>() 
	{
		return DynamicTypes::String;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<DynamicList>() 
	{
		return DynamicTypes::List;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<DynamicMap>() 
	{
		return DynamicTypes::Map;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<void*>() 
	{
		return DynamicTypes::Pointer;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<Date>()
	{
		return DynamicTypes::Date;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<Time>()
	{
		return DynamicTypes::Time;
	}
	template<>
	DynamicTypes::Value Dynamic::ResolveType<DateTime>()
	{
		return DynamicTypes::DateTime;
	}
}
