#ifndef STDUTILS_PERSISTANCE_IPERSISTANTMAP_H
#define STDUTILS_PERSISTANCE_IPERSISTANTMAP_H


#include <StdUtils/Dynamic.h>


namespace StdUtils
{
	namespace Persistance
	{
		class IPersistantMap
		{
		public:
			virtual ~IPersistantMap() {}

		private:
			virtual Dynamic resolvePersistantValue(String const& fieldName, DynamicTypes::Value valueType) const = 0;

		public:
			virtual bool isNull(String const& fieldName) const = 0;
			template<typename T>
			T getPersistantValue(String const& fieldName) const
			{
				return this->resolvePersistantValue(fieldName, Dynamic::ResolveType<T>()).getValue<T>();
			}
		};
	}
}

#endif