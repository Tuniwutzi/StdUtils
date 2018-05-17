#include <StdUtils/Base.h>

#include <StdUtils/Threading/Definitions.h>

namespace StdUtils
{
    namespace Threading
    {
#ifdef _WINDOWS
        typedef HANDLE SemaphoreHandle;
#else
        typedef sem_t SemaphoreHandle;
#endif
        class STDUTILS_DLLCONTENT Semaphore
        {
        protected:
            SemaphoreHandle handle;

        public:
            Semaphore();
            ~Semaphore();

        public:
            void wait();
            void signal();
        };
    }
}