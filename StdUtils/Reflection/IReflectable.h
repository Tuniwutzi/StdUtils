#ifndef STDUTILS_REFLECTION_IREFLECTABLE_H
#define STDUTILS_REFLECTION_IREFLECTABLE_H


#include <StdUtils/Base.h>
#include <StdUtils/Reflection/Member.h>

#include <vector>


namespace StdUtils
{
	namespace Reflection
	{
		typedef std::vector<IField*> Fieldlist;
		typedef std::vector<IMember*> Memberlist;

		class IReflectable
		{
		public:
			virtual ~IReflectable() {}

		public:
			virtual Memberlist const& getMembers() = 0;
		};
	}
}


#endif