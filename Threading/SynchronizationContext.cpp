#include <StdUtils/Threading/SynchronizationContext.h>
#include <StdUtils/Threading/Mutex.h>

namespace StdUtils
{
	namespace Threading
	{
		template<>
		SynchronizationContext<Mutex>::SynchronizationContext(Mutex& lock)
			:synchronizer(lock)
		{
			this->synchronizer.lock();
		}
		template<>
		SynchronizationContext<Mutex>::~SynchronizationContext()
		{
			this->synchronizer.release();
		}
	}
}