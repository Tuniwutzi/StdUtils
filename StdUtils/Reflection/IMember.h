#ifndef STDUTILS_REFLECTION_IMEMBER_H
#define STDUTILS_REFLECTION_IMEMBER_H


#include <StdUtils/Reflection/Field.h>


namespace StdUtils
{
	namespace Reflection
	{
		class IReflectable;

		class IMember
		{
		protected:
			IReflectable* instance;
			IField* field;

		protected:
			IMember(IReflectable* instance, IField* field)
				:instance(instance),
				field(field)
			{
			}
		public:
			virtual ~IMember() {}

		public:
			std::string const& getName() const
			{
				return this->field->getName();
			}
			DynamicTypes::Value getType() const
			{
				return this->field->getType();
			}
			bool isNullable() const
			{
				return this->field->isNullable();
			}
			IField* getField()
			{
				return this->field;
			}
			IReflectable* getInstance()
			{
				return this->instance;
			}

			virtual IMember* duplicate(IReflectable* instance) const = 0;

			virtual Dynamic getValue() const = 0;
			virtual void setValue(Dynamic const&) = 0;
		};
	}
}



#endif