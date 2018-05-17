#ifndef STDUTILS_THREADING_MANAGEDSEMAPHORE_H
#define STDUTILS_THREADING_MANAGEDSEMAPHORE_H

#include <StdUtils/Threading/Semaphore.h>
#include <StdUtils/Threading/Mutex.h>
#include <StdUtils/Threading/Thread.h>

namespace StdUtils
{
	namespace Threading
	{
		class ManagedSemaphore : private Semaphore
		{
		protected:
			Mutex lock;
			Thread::ThreadId holdingThread;
			
			int waiting;
			int count;

		public:
			ManagedSemaphore();

		private:
			void checkLock() const;
			void doSignal();

		public:
			void acquireLock();
			void releaseLock();

			void wait();

			void signal();
			void signalAll();
		};
	}
}

#endif