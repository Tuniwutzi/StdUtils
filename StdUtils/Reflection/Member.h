#ifndef STDUTILS_REFLECTION_MEMBER_H
#define STDUTILS_REFLECTION_MEMBER_H


#include <StdUtils/Reflection/IMember.h>


namespace StdUtils
{
	namespace Reflection
	{
		template<typename FIELDTYPE, typename MODELTYPE>
		class Member : public IMember
		{
		public:
			Member(MODELTYPE* instance, Field<FIELDTYPE, MODELTYPE>* field)
				:IMember(instance, field)
			{
			}

		public:
			virtual ~Member() {}

		public:
			virtual IMember* duplicate(IReflectable* instance) const
			{
				MODELTYPE* inst = dynamic_cast<MODELTYPE*>(instance);
				if (inst)
					return new Member<FIELDTYPE, MODELTYPE>(inst, (Field<FIELDTYPE, MODELTYPE>*)this->field);
				else
					throw Exception("Invalid type");
			}

			virtual Dynamic getValue() const
			{
				return this->get();
			}
			virtual void setValue(Dynamic const& value)
			{
				this->set(value.getValue<FIELDTYPE>());
			}

			FIELDTYPE const& get() const
			{
				Field<FIELDTYPE, MODELTYPE>* typedField = dynamic_cast<Field<FIELDTYPE, MODELTYPE>*>(this->field);
				if (typedField)
					return typedField->get(dynamic_cast<MODELTYPE*>(this->instance));
				else
					throw Exception("Type error");
			}
			void set(FIELDTYPE const& value)
			{
				Field<FIELDTYPE, MODELTYPE>* typedField = dynamic_cast<Field<FIELDTYPE, MODELTYPE>*>(this->field);
				if (typedField)
					typedField->set(value, dynamic_cast<MODELTYPE*>(this->instance));
				else
					throw Exception("Type error");
			}
		};
		template<typename FIELDTYPE, class MODELTYPE>
		class Member<Nullable<FIELDTYPE>, MODELTYPE> : public IMember
		{
		public:
			Member(MODELTYPE* instance, Field<Nullable<FIELDTYPE>, MODELTYPE>* field)
				:IMember(instance, field)
			{
			}

		public:
			virtual ~Member() {}

		public:
			virtual IMember* duplicate(IReflectable* instance) const
			{
				MODELTYPE* inst = dynamic_cast<MODELTYPE*>(instance);
				if (inst)
					return new Member<Nullable<FIELDTYPE>, MODELTYPE>(inst, (Field<Nullable<FIELDTYPE>, MODELTYPE>*)this->field);
				else
					throw Exception("Invalid type");
			}

			virtual Dynamic getValue() const
			{
				Nullable<FIELDTYPE> const& val = this->get();
				return (val.hasValue() ? val.getValue() : Dynamic());
			}
			virtual void setValue(Dynamic const& value)
			{
				this->set((value.isNull() ? Nullable<FIELDTYPE>() : value.getValue<FIELDTYPE>()));
			}

			Nullable<FIELDTYPE> const& get() const
			{
				Field<Nullable<FIELDTYPE>, MODELTYPE>* typedField = dynamic_cast<Field<Nullable<FIELDTYPE>, MODELTYPE>*>(this->field);
				if (typedField)
					return typedField->get(dynamic_cast<MODELTYPE*>(this->instance));
				else
					throw Exception("Type error");
			}
			void set(Nullable<FIELDTYPE> const& value)
			{
				Field<Nullable<FIELDTYPE>, MODELTYPE>* typedField = dynamic_cast<Field<Nullable<FIELDTYPE>, MODELTYPE>*>(this->field);
				if (typedField)
					typedField->set(value, dynamic_cast<MODELTYPE*>(this->instance));
				else
					throw Exception("Type error");
			}
		};
	}
}

#endif