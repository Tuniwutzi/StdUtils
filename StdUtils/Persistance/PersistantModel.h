#ifndef STDUTILS_PERSISTANCE_IPERSISTANTMODEL_H
#define STDUTILS_PERSISTANCE_IPERSISTANTMODEL_H


#include <vector>

#include <StdUtils/Reflection/Reflectable.h>
#include <StdUtils/DateTime.h>
#include <StdUtils/Persistance/IPersistantMap.h>


namespace StdUtils
{
	namespace Persistance
	{
		STDUTILS_START_FLAGS(PersistantFieldAttributes)
		{
			None		= 0x00000000,

				IgnoreOnInsert = 0x0001,
				ReturnOnInsert = 0x0003,

				IgnoreOnSelect = 0x0004,

				//Default timezone is UTC
				TimezoneLocal = 0x00010000
		}
		STDUTILS_END_FLAGS(PersistantFieldAttributes);

		class IPersistantField
		{
		private:
			PersistantFieldAttributes attributes;

		public:
			IPersistantField(PersistantFieldAttributes::Value a)
				: attributes(a)
			{
			}
			virtual ~IPersistantField()
			{
			}
		public:
			virtual void setValue(StdUtils::Reflection::IReflectable& instance, IPersistantMap&) = 0;
			virtual StdUtils::Reflection::IField const& getIField() const = 0;
			virtual StdUtils::Reflection::IField& getIField() = 0;

			PersistantFieldAttributes const& getAttributes() const
			{
				return this->attributes;
			}
		};

		template<typename FIELDTYPE>
		static FIELDTYPE DoGet(StdUtils::String const& name, PersistantFieldAttributes const& a, IPersistantMap& map)
		{
			return map.getPersistantValue<FIELDTYPE>(name);
		}
		template<>
		static StdUtils::DateTime DoGet(StdUtils::String const& name, PersistantFieldAttributes const& a, IPersistantMap& map)
		{
			StdUtils::Timezone const* tz(&StdUtils::Timezone::Utc());
			if (a.allSet(PersistantFieldAttributes::TimezoneLocal))
				tz = &StdUtils::Timezone::Local();

			//return map.getDateTime(name, *tz);
			DateTime rv(map.getPersistantValue<DateTime>(name));

			return tz->convert(rv);
		}
		template<typename FIELDTYPE, class MODELTYPE>
		class PersistantField : public IPersistantField, public StdUtils::Reflection::Field<FIELDTYPE, MODELTYPE>
		{
		public:
			PersistantField(StdUtils::String const& name, typename StdUtils::Reflection::Field<FIELDTYPE, MODELTYPE>::PointerToMember member, PersistantFieldAttributes::Value a)
				: IPersistantField(a),
				StdUtils::Reflection::Field<FIELDTYPE, MODELTYPE>(name, member)
			{
			}

		public:
			virtual void setValue(StdUtils::Reflection::IReflectable& instance, IPersistantMap& map) STDUTILS_OVERRIDE
			{
				MODELTYPE* mt = dynamic_cast<MODELTYPE*>(&instance);
				if (mt)
				{
					FIELDTYPE val(DoGet<FIELDTYPE>(StdUtils::Reflection::Field<FIELDTYPE, MODELTYPE>::getName(), this->getAttributes(), map));
					//				FIELDTYPE val = row.get<FIELDTYPE>(StdUtils::Reflection::Field<FIELDTYPE, MODELTYPE>::getName());
					this->set(val, mt);
				}
				else
					throw Exception("Instance is not of a valid type");
			}

			virtual StdUtils::Reflection::IField const& getIField() const
			{
				return *this;
			}
			virtual StdUtils::Reflection::IField& getIField()
			{
				return *this;
			}
		};

		template<typename FIELDTYPE, class MODELTYPE>
		class PersistantField<StdUtils::Nullable<FIELDTYPE>, MODELTYPE> : public IPersistantField, public StdUtils::Reflection::Field<
			StdUtils::Nullable<FIELDTYPE>, MODELTYPE>
		{
		public:
			PersistantField(StdUtils::String const& name, typename StdUtils::Reflection::Field<StdUtils::Nullable<FIELDTYPE>,
				MODELTYPE>::PointerToMember member, PersistantFieldAttributes::Value a)
				: IPersistantField(a),
				StdUtils::Reflection::Field<StdUtils::Nullable<FIELDTYPE>, MODELTYPE>(name, member)
			{
			}

		public:
			virtual void setValue(StdUtils::Reflection::IReflectable& instance, IPersistantMap& map) STDUTILS_OVERRIDE
			{
				MODELTYPE* mt = dynamic_cast<MODELTYPE*>(&instance);
				if (mt)
				{
					if (!map.isNull(StdUtils::Reflection::Field<StdUtils::Nullable<FIELDTYPE>, MODELTYPE>::getName()))
					{
						FIELDTYPE val(DoGet<FIELDTYPE>(StdUtils::Reflection::Field<StdUtils::Nullable<FIELDTYPE>, MODELTYPE>::getName(), this->getAttributes(), map));
						this->set(val, mt);
					}
					else
						this->set(StdUtils::Nullable<FIELDTYPE>(), mt);
				}
				else
					throw Exception("Instance is not of a valid type");
			}

			virtual StdUtils::Reflection::IField const& getIField() const
			{
				return *this;
			}
			virtual StdUtils::Reflection::IField& getIField()
			{
				return *this;
			}
		};

		typedef std::vector<IPersistantField*> PersistantFields;
		template<class MODELTYPE>
		class PersistantModel: public StdUtils::Reflection::Reflectable<MODELTYPE>
		{
			friend class StdUtils::Reflection::ReflectableInitializer<MODELTYPE>;

		private:
			struct PersistanceMetadata
			{
				PersistantFields Fields;
				PersistanceMetadata(StdUtils::Reflection::Fieldlist const& fields)
				{
					for (StdUtils::Reflection::Fieldlist::const_iterator it = fields.begin(); it != fields.end(); it++)
					{
						StdUtils::Reflection::IField* fld = *it;
						IPersistantField* dbfld = dynamic_cast<IPersistantField*>(fld);
						if (dbfld)
							this->Fields.push_back(dbfld);
					}
				}
			};

		protected:
			template<typename FIELDTYPE>
			static PersistantField<FIELDTYPE, MODELTYPE>& addPersistantField(StdUtils::String const& name, typename StdUtils::Reflection::Field<
				FIELDTYPE, MODELTYPE>::PointerToMember member, PersistantFieldAttributes::Value a = PersistantFieldAttributes::None)
			{
				PersistantField<FIELDTYPE, MODELTYPE>* field = new PersistantField<FIELDTYPE, MODELTYPE>(name, member, a);
				//PersistantModel<MODELTYPE>::dbFields.push_back(field);
				StdUtils::Reflection::Reflectable<MODELTYPE>::addField(field);
				return *field;
			}

		public:
			virtual ~PersistantModel()
			{
			}

		public:
			static PersistantFields const& GetPersistantFields()
			{
				static PersistanceMetadata meta(StdUtils::Reflection::Reflectable<MODELTYPE>::GetFields());
				return meta.Fields;
			}
			static IPersistantField& GetPersistantField(StdUtils::String const& name)
			{
				static PersistantFields const& fields(GetPersistantFields());
				for (PersistantFields::const_iterator it = fields.begin(); it != fields.end(); it++)
				{
					if ((*it)->getIField().getName() == name)
						return **it;
				}

				throw Exception(String("Field ") + GetTypeName() + "::" + name + " not found");
			}
		};
	}
}

#endif
