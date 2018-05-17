#ifndef STDUTILS_REFLECTION_FIELD_H
#define STDUTILS_REFLECTION_FIELD_H


#include <StdUtils/Reflection/IField.h>


namespace StdUtils
{
	namespace Reflection
	{
		template<typename FIELDTYPE, typename MODELTYPE>
		class Field : public IField
		{
		public:
			typedef FIELDTYPE MODELTYPE::* PointerToMember;

		private:
			PointerToMember member;

		public:
			Field(String const& name, PointerToMember member)
				:IField(name, Dynamic::ResolveType<FIELDTYPE>(), false),
				member(member)
			{
			}
		public:
			virtual ~Field()
			{
			}

		public:
			virtual Dynamic getValue(IReflectable* instance) const
			{
				MODELTYPE* inst = dynamic_cast<MODELTYPE*>(instance);
				return this->get(inst);
			}
			virtual void setValue(Dynamic const& value, IReflectable* instance)
			{
				MODELTYPE* inst = dynamic_cast<MODELTYPE*>(instance);
				this->set(value.getValue<FIELDTYPE>(), inst);
			}

			FIELDTYPE const& get(MODELTYPE* instance) const
			{
				if (instance)
					return *instance.*this->member;
				else
					throw Exception("Invalid instance pointer");
			}
			void set(FIELDTYPE const& value, MODELTYPE* instance)
			{
				if (instance)
					*instance.*this->member = value;
				else
					throw Exception("Invalid instance pointer");
			}
		};

		template<typename FIELDTYPE, typename MODELTYPE>
		class Field<Nullable<FIELDTYPE>, MODELTYPE> : public IField
		{
		public:
			typedef Nullable<FIELDTYPE> MODELTYPE::* PointerToMember;

		private:
			PointerToMember member;

		public:
			Field(String const& name, PointerToMember member)
				:IField(name, Dynamic::ResolveType<FIELDTYPE>(), true),
				member(member)
			{
			}
		public:
			virtual ~Field()
			{
			}

		public:
			virtual Dynamic getValue(IReflectable* instance) const
			{
				MODELTYPE* inst = dynamic_cast<MODELTYPE*>(instance);
				Nullable<FIELDTYPE> const& val = this->get(inst);
				if (val.hasValue())
					return val.getValue();
				else
					return Dynamic();
			}
			virtual void setValue(Dynamic const& value, IReflectable* instance)
			{
				MODELTYPE* inst = dynamic_cast<MODELTYPE*>(instance);
				if (value.isNull())
					this->set(Nullable<FIELDTYPE>(), inst);
				else
					this->set(value.getValue<FIELDTYPE>(), inst);
			}

			Nullable<FIELDTYPE> const& get(MODELTYPE* instance) const
			{
				if (instance)
					return *instance.*this->member;
				else
					throw Exception("Invalid instance pointer");
			}
			void set(Nullable<FIELDTYPE> const& value, MODELTYPE* instance)
			{
				if (instance)
					*instance.*this->member = value;
				else
					throw Exception("Invalid instance pointer");
			}
		};
	}
}

#endif