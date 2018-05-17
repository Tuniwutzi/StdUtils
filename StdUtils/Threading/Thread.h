#ifndef STDUTILS_THREADING_H
#define STDUTILS_THREADING_H


#include <map>
#include <vector>

#include <StdUtils/Delegates.h>
#include <StdUtils/Threading/Mutex.h>
#include <StdUtils/Threading/Exceptions.h>


namespace StdUtils
{
    namespace Threading
    {
        class STDUTILS_DLLCONTENT Thread
        {
        private:
            struct ThreadStartArgs
            {
                Thread* Threadobject;
                void* ActualArgs;
            };


        public:
#ifdef _WINDOWS
            typedef HANDLE ThreadHandle;
			typedef DWORD ThreadId;
#else
            typedef pthread_t ThreadHandle;
			typedef pthread_t ThreadId;
#endif

			typedef Delegate<void, Thread*> ThreadEndHandler;
			typedef std::vector<ThreadEndHandler> ThreadEndHandlers;

		private:
			typedef std::map<ThreadId, Thread*> Threadlist;

        private:
			bool running;
			bool freed;

            ThreadHandle threadHandle;
			ThreadId threadId;

            Delegate<void*, void*> function;
			ThreadEndHandlers endHandlers; 
			Mutex handlerLock;

        private:
            Thread(Thread const & original);
            Thread& operator=(Thread const & original);

        private:
            void freeHandle();

            void assertRunning() const;
			void assertNotRunning() const;

        public:
            Thread(Delegate<void*, void*> function);
            ~Thread();

			//The handler is called from inside the spawned thread
			void addThreadEndHandler(ThreadEndHandler const&);
			void removeThreadEndHandler(ThreadEndHandler const&);

            void start(void* args = NULL);
            void stop();

            bool isRunning() const;

            void waitForTermination(DWORD timeoutMS = INFINITE) const;

		private:
			static Mutex threadlistLock;
			static Threadlist threads;

        private:
            static void* threadStart(void* args);
            
		public:
			static const ThreadId NoThread;

		public:
            static void Sleep(uint64_t ms);
			static ThreadId CurrentId();
			static Thread* Current();
        };
    }
}
#endif
