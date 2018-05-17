#include <StdUtils/Threading/Mutex.h>


#include <StdUtils/Exceptions.h>

//TODO: Besseres errorhandling
namespace StdUtils
{
   namespace Threading
   {
      //Mutex
      Mutex::Mutex()
			:locked(false)
      {
         bool error = false;
#ifdef _WINDOWS
         MHandle = CreateMutex(0, false, 0);
         error = MHandle == INVALID_HANDLE_VALUE;
#else
		 //Make mutex recursive
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
         int returnvalue = pthread_mutex_init(&MHandle, &attr);
         error = returnvalue != 0;
#endif
         if (error)
            throw Exception("Mutex could not be created");
      }
      Mutex::~Mutex()
      {
         //TODO: Fehlerrueckgabe verarbeiten. Moeglicherweise ist der Mutex noch gelockt und kann deshalb nicht zerstoert werden
         //Dazu: Keine Exceptions aus dem Destruktor werfen!
			if (this->locked)
				this->release();

#ifdef _WINDOWS
         CloseHandle(MHandle);
#else
         pthread_mutex_destroy(&MHandle);
#endif
      }

      void Mutex::lock()
      {
#ifdef _WINDOWS
         WaitForSingleObject(MHandle, INFINITE);
#else
         pthread_mutex_lock(&MHandle);
#endif
			this->locked = true;
      }

      void Mutex::release()
      {
#ifdef _WINDOWS
         ReleaseMutex(MHandle);
#else
         pthread_mutex_unlock(&MHandle);
#endif
			this->locked = false;
      }
   }
}
