#ifndef STDUTILS_SYNCHRONIZATIONCONTEXT_H
#define STDUTILS_SYNCHRONIZATIONCONTEXT_H

#include <StdUtils/Base.h>

namespace StdUtils
{
	namespace Threading
	{
		template<class T>
		class STDUTILS_DLLCONTENT SynchronizationContext
		{
		private:
			T& synchronizer;

		public:
			SynchronizationContext(T& synchronizer);
			~SynchronizationContext();
		};
	}
}

#endif