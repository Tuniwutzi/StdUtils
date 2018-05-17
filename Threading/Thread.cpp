#include <StdUtils/Threading/Thread.h>


#include <StdUtils/Threading/Mutex.h>

#ifndef _WINDOWS
#include <unistd.h>
#endif


namespace StdUtils
{
	namespace Threading
	{
		Thread::Thread(Delegate<void*, void*> function)
			:running(false),
			freed(true),
			function(function)
		{
		}
		Thread::~Thread()
		{
			if (this->isRunning())
				this->stop();
			if (!this->freed)
				this->freeHandle();
		}

		void Thread::assertRunning() const
		{
			if (!this->running)
				throw Exception("Thread is not running");
		}
		void Thread::assertNotRunning() const
		{
			if (this->running)
				throw Exception("Thread is already running");
		}

		void Thread::freeHandle()
		{
			this->waitForTermination();

#ifdef _WINDOWS
			CloseHandle(this->threadHandle);
#else
			//With POSIX threads pthread_join does this automatically (called in WaitForTermination)
#endif

			this->freed = true;
		}

		void Thread::addThreadEndHandler(ThreadEndHandler const& handler)
		{
			STDUTILS_SYNCHRONIZE(this->handlerLock);

			this->endHandlers.push_back(handler);
		}
		void Thread::removeThreadEndHandler(ThreadEndHandler const& handler)
		{
			STDUTILS_SYNCHRONIZE(this->handlerLock);

			for (ThreadEndHandlers::iterator it = this->endHandlers.begin(); it != this->endHandlers.end(); it++)
			{
				if ((*it) == handler)
				{
					this->endHandlers.erase(it);
					return;
				}
			}
		}

		void Thread::stop()
		{
			this->assertRunning();

#ifdef _WINDOWS
			TerminateThread(threadHandle, 0);
#else
			pthread_cancel(threadHandle);
#endif
		}

		void Thread::start(void* args)
		{
			this->assertNotRunning();

			if (!this->freed)
				this->freeHandle();

			ThreadStartArgs* tsa = new ThreadStartArgs;
			tsa->ActualArgs = args;
			tsa->Threadobject = this;

#ifdef _WINDOWS
			this->threadHandle = CreateThread(0, 2000, (LPTHREAD_START_ROUTINE)Thread::threadStart, tsa, 0, NULL);
			this->running = threadHandle != INVALID_HANDLE_VALUE;
#else
			this->running = pthread_create(&this->threadHandle, NULL, &Thread::threadStart, tsa) == 0; //0 on success, otherwise errorcode
#endif
			
			this->freed = !this->running;
			if (!this->running)
				throw BasicThreadException("Could not create thread");
		}

		bool Thread::isRunning() const
		{
			return this->running;
		}

#ifndef _WINDOWS

		struct JoinArgs
		{
			Thread::ThreadHandle Handle;
			bool Joined;
		};
		static void* pthread_join_timeout_helper(void* joinArgsO)
		{
			JoinArgs* joinArgs = (JoinArgs*)joinArgsO;

			pthread_join(joinArgs->Handle, NULL);

			joinArgs->Joined = true;

			return NULL;
		}
		static bool pthread_join_timeout(Thread::ThreadHandle thread, DWORD timeoutMS)
		{
			if (timeoutMS != INFINITE)
			{
				if (timeoutMS < 600)
					timeoutMS = 600; //so the helperthread will have enough time to execute

				Thread::ThreadHandle helperHandle;

				JoinArgs args;
				args.Handle = thread;
				args.Joined = false;

				if (pthread_create(&helperHandle, NULL, &pthread_join_timeout_helper, &args))
					return false;

				timespec starttime;
				if (clock_gettime(CLOCK_REALTIME, &starttime))
					throw BasicThreadException("Unknown error");

				timespec currtime;
				DWORD diffMSecs;

				//TODO: Performance kontrollieren... yield effektiv?
				do
				{
					if (sched_yield()) //0 on success
						sleep(1); //just sleep if yield fails, time in seconds

					if (clock_gettime(CLOCK_REALTIME, &currtime))
						throw BasicThreadException("Unknown error");
					else
					{
						diffMSecs = (currtime.tv_sec - starttime.tv_sec) * 1000;
						diffMSecs += (currtime.tv_nsec - starttime.tv_nsec) / 1000000;

						if (diffMSecs >= timeoutMS)
							break;
					}
				} while (!args.Joined);

				if (!args.Joined)
					pthread_cancel(helperHandle);

				pthread_join(helperHandle, NULL);

				return args.Joined;
			}
			else
				pthread_join(thread, NULL);

			return true;
		}

#endif

		void Thread::waitForTermination(DWORD timeoutMS) const
		{
			if (this->isRunning())
			{
#ifdef _WINDOWS
				switch (WaitForSingleObject(threadHandle, timeoutMS))
				{
				case WAIT_OBJECT_0:
					//Success
					break;
				case WAIT_TIMEOUT:
					throw TimeoutException();
					break;
				default:
					throw BasicThreadException("Unknown error");
					break;
				}
#else
				if (!pthread_join_timeout(threadHandle, timeoutMS))
					throw TimeoutException();
#endif
			}
		}

		Mutex Thread::threadlistLock;
		Thread::Threadlist Thread::threads;
		void* Thread::threadStart(void* args)
		{
			ThreadStartArgs* tsa = (ThreadStartArgs*)args;

			ThreadId myId = Thread::CurrentId();
			tsa->Threadobject->threadId = myId;

			STDUTILS_LOCK(Thread::threadlistLock)
			{
				Threadlist::iterator it = Thread::threads.find(myId);
				if (it != Thread::threads.end())
				{
					//TODO: Merkwuerdiger fehler, threadid doppelt. Wie reagieren? Exception geht nicht, wuerde IMMER das Programm zum Absturz bringen
				}
				else
				{
					Thread::threads[myId] = tsa->Threadobject;
				}
			}
			STDUTILS_RELEASE(Thread::threadlistLock);

			void* rv = tsa->Threadobject->function(tsa->ActualArgs);

			STDUTILS_LOCK(Thread::threadlistLock)
			{
				Thread::threads.erase(myId);
			}
			STDUTILS_RELEASE(Thread::threadlistLock);

			try
			{
				STDUTILS_SYNCHRONIZE(tsa->Threadobject->handlerLock);
				for (ThreadEndHandlers::iterator it = tsa->Threadobject->endHandlers.begin(); it != tsa->Threadobject->endHandlers.end(); it++)
					it->operator()(tsa->Threadobject);
			}
			catch (...)
			{
			}
			tsa->Threadobject->running = false;

			return rv;
		}

		const Thread::ThreadId Thread::NoThread(0);

#ifdef _WINDOWS
		static void doSleep(uint64_t ms)
		{
			while (ms > MAXDWORD)
			{
				Sleep(MAXDWORD);
				ms -= MAXDWORD;
			}
			Sleep((DWORD)ms);
		}
#endif
		void Thread::Sleep(uint64_t ms)
		{
#ifdef _WINDOWS
			doSleep(ms);
#else
			usleep(ms * 1000);
#endif
		}
		Thread::ThreadId Thread::CurrentId()
		{
#ifdef _WINDOWS
			return GetCurrentThreadId();
#else
			return pthread_self();
#endif
		}
		Thread* Thread::Current()
		{
			Thread* rv = NULL;

			STDUTILS_LOCK(Thread::threadlistLock)
			{
				Threadlist::iterator it = Thread::threads.find(Thread::CurrentId());
				if (it != Thread::threads.end())
					rv = it->second;
			}
			STDUTILS_RELEASE(Thread::threadlistLock);

			return rv;
		}
	}
}
