#include <StdUtils/Threading/ManagedSemaphore.h>

namespace StdUtils
{
	namespace Threading
	{
		ManagedSemaphore::ManagedSemaphore()
			:holdingThread(Thread::NoThread),
			waiting(0),
			count(0)
		{
		}


		void ManagedSemaphore::checkLock() const
		{
			if (this->holdingThread != Thread::CurrentId())
				throw Exception("Must aquire lock before waiting or signaling");
		}
		void ManagedSemaphore::doSignal()
		{
			this->count++;
			Semaphore::signal();
		}


		void ManagedSemaphore::acquireLock()
		{
			this->lock.lock();
			this->holdingThread = Thread::CurrentId();
		}
		void ManagedSemaphore::releaseLock()
		{
			this->holdingThread = Thread::NoThread;
			this->lock.release();
		}

		void ManagedSemaphore::wait()
		{
			this->checkLock();

			this->waiting++;
			this->releaseLock();

			Semaphore::wait();

			this->acquireLock();
			this->waiting--;
			this->count--;
		}

		void ManagedSemaphore::signal()
		{
			this->checkLock();

			this->doSignal();
		}
		void ManagedSemaphore::signalAll()
		{
			this->checkLock();

			while (this->count < this->waiting)
				this->doSignal();
		}
	}
}