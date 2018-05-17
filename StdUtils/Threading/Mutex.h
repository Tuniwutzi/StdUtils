#ifndef STDUTILS_THREADING_MUTEX
#define STDUTILS_THREADING_MUTEX

#include <StdUtils/Base.h>
#include <StdUtils/Threading/SynchronizationContext.h>
#include <StdUtils/Threading/Definitions.h>

#define STDUTILS_SYNCHRONIZE(lock) StdUtils::Threading::MutexSynchronizer synchronized(lock)

namespace StdUtils
{
	namespace Threading
	{
#ifdef _WINDOWS
		typedef HANDLE MutexHandle;
#else
		typedef pthread_mutex_t MutexHandle;
#endif
		class STDUTILS_DLLCONTENT Mutex
		{
		private:
			MutexHandle MHandle;
			bool locked;

		public:
			Mutex();
			~Mutex();

        public:
			void lock();
			void release();
		};

		typedef SynchronizationContext<Mutex> MutexSynchronizer;
	}
}

#endif