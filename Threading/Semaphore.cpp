#include <StdUtils/Threading/Semaphore.h>


#include <StdUtils/Exceptions.h>


namespace StdUtils
{
	namespace Threading
	{
#ifdef _WINDOWS
		Semaphore::Semaphore()
			:handle(CreateSemaphore(0, 0, 0x7FFFFFFF, 0))
		{
			if (!this->handle)
				throw OSApiException(GetLastError());
		}
#else
		Semaphore::Semaphore()
		{
			if (sem_init(&this->handle, false, 0))
				throw OSApiException();
		}
#endif

		Semaphore::~Semaphore()
		{
#ifdef _WINDOWS
			CloseHandle(this->handle);
#else
			sem_destroy(&this->handle);
#endif
		}

		void Semaphore::wait()
		{
#ifdef _WINDOWS
			WaitForSingleObject(this->handle, INFINITE);
#else
			sem_wait(&this->handle);
#endif
		}

		void Semaphore::signal()
		{
#ifdef _WINDOWS
			ReleaseSemaphore(this->handle, 1, NULL);
#else
			sem_post(&this->handle);
#endif
		}
	}
}
