#ifndef STDUTILS_REFLECTION_IFIELD_H
#define STDUTILS_REFLECTION_IFIELD_H


#include <StdUtils/Base.h>
#include <StdUtils/Dynamic.h>


namespace StdUtils
{
	namespace Reflection
	{
		class IReflectable;

		class IField
		{
		private:
			String name;
			DynamicTypes::Value type;
			bool nullable;

		protected:
			IField(String const& name, DynamicTypes::Value const& type, bool nullable)
				:name(name),
				type(type),
				nullable(nullable)
			{
			}
		public:
			virtual ~IField() {}

		public:
			String const& getName() const
			{
				return this->name;
			}
			DynamicTypes::Value getType() const
			{
				return this->type;
			}
			bool isNullable() const
			{
				return this->nullable;
			}

			virtual Dynamic getValue(IReflectable* instance) const = 0;
			virtual void setValue(Dynamic const&, IReflectable* instance) = 0;
		};
	}
}


#endif