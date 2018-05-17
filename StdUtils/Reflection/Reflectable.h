#ifndef STDUTILS_REFLECTABLE_H
#define STDUTILS_REFLECTABLE_H


#include <StdUtils/Reflection/IReflectable.h>


namespace StdUtils
{
	namespace Reflection
	{
		template<class MODELTYPE>
		class ReflectableInitializer
		{
			template<class> friend class Reflectable;

		private:
			static void initializeReflectable(String& typeNameBuffer);
		};

		template<class MODELTYPE>
		class Reflectable : public IReflectable
		{
			template<class> friend class ReflectableInitializer;

		private:
			class Metadata
			{
			public:
				String TypeName;
				Fieldlist Fields;
				Memberlist PseudoMembers;
				
			public:
				Metadata()
					:TypeName("UnknownTypeName")
				{
				}

			public:
				~Metadata()
				{
					for (Fieldlist::iterator it = this->Fields.begin(); it != this->Fields.end(); it++)
						delete *it;
					for (Memberlist::iterator it = this->PseudoMembers.begin(); it != this->PseudoMembers.end(); it++)
						delete *it;
				}

			public:
				void init()
				{
					ReflectableInitializer<MODELTYPE>::initializeReflectable(TypeName);
				}
			};

		private:
			bool membersInitialized;
			Memberlist members;

		protected:
			Reflectable()
				:membersInitialized(false)
			{
			}
		public:
			Reflectable(Reflectable<MODELTYPE> const& o)
				:membersInitialized(false)
			{
			}

		public:
			virtual ~Reflectable()
			{
				for (Memberlist::iterator it = this->members.begin(); it != this->members.end(); it++)
					delete *it;
			}

		public:
			Reflectable<MODELTYPE>& operator=(Reflectable<MODELTYPE> const& o)
			{
				this->membersInitialized = false;

				for (Memberlist::iterator it = this->members.begin(); it != this->members.end(); it++)
					delete *it;
				this->members.clear();

				return *this;
			}

		public:
			virtual Memberlist const& getMembers()
			{
				//TODO: Nicht Threadsafe - Problem?
				if (!this->membersInitialized)
				{
					this->membersInitialized = true;

					Metadata& md(getMetadata());

					for (Memberlist::const_iterator it = md.PseudoMembers.begin(); it != md.PseudoMembers.end(); it++)
						this->members.push_back((*it)->duplicate(this));
				}
				return this->members;
			}


		private:
			static Metadata& getMetadata()
			{
				static Metadata* md(NULL);
				if (!md)
				{
					md = new Metadata();
					md->init();
				}

				return *md;
			}

		protected:
			template<typename FIELDTYPE>
			static Field<FIELDTYPE, MODELTYPE>& addField(Field<FIELDTYPE, MODELTYPE>* field)
			{
				Metadata& md(getMetadata());

				md.Fields.push_back(field);
				md.PseudoMembers.push_back(new Member<FIELDTYPE, MODELTYPE>(NULL, field)); //TODO: Hacky
				return *field;
			}
			template<typename FIELDTYPE>
			static Field<FIELDTYPE, MODELTYPE>& addField(String const& name, typename Field<FIELDTYPE, MODELTYPE>::PointerToMember member)
			{
				Field<FIELDTYPE, MODELTYPE>* field = new Field<FIELDTYPE, MODELTYPE>(name, member);
				Reflectable<MODELTYPE>::addField<FIELDTYPE>(field);
				return *field;
			}

		public:
			static std::vector<IField*> const& GetFields()
			{
				static Metadata& md(getMetadata());

				return md.Fields;
			}
			static String const& GetTypeName()
			{
				static Metadata& md(getMetadata());

				return md.TypeName;
			}
		};
	}
}

#endif