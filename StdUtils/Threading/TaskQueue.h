#ifndef STDUTILS_TASKQUEUE_H
#define STDUTILS_TASKQUEUE_H

#include <StdUtils/Base.h>
#include <StdUtils/Threading/Thread.h>
#include <StdUtils/Threading/Mutex.h>
#include <StdUtils/Threading/Semaphore.h>
#include <StdUtils/Dynamic.h>
#include <StdUtils/Delegates.h>

#include <queue>

namespace StdUtils
{
	namespace Threading
	{
		class TaskQueue
		{
			STDUTILS_PREVENT_ASSIGNMENT(TaskQueue);
			STDUTILS_PREVENT_COPY(TaskQueue);

		private:
			struct Task
			{
				StdUtils::Delegate<void, StdUtils::Dynamic&> Worker;
				StdUtils::Dynamic Parameter;
				Task(StdUtils::IDelegate<void, StdUtils::Dynamic&> const&, StdUtils::Dynamic const&);
			};
		private:
			std::queue<Task> tasks;
			Thread thread;

			Semaphore semaphore;
			Mutex controlLock;
			Mutex workerLock;

			long maxTasks;
			bool running;

		public:
			TaskQueue();
			~TaskQueue();

		private:
			void* worker(void*);

		public:
			void start();
			void stop();

			void enqueue(IDelegate<void, Dynamic&>& worker, Dynamic const& parameter);
		};
	}
}

#endif