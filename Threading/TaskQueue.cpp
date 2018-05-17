#include <StdUtils/Threading/TaskQueue.h>


#include <StdUtils/Threading/SynchronizationContext.h>


namespace StdUtils
{
	namespace Threading
	{
		TaskQueue::Task::Task(StdUtils::IDelegate<void, StdUtils::Dynamic&> const& worker, StdUtils::Dynamic const& parameter)
		{
		}

		TaskQueue::TaskQueue()
			:thread(Delegate<void*, void*>(DelegateMethod<TaskQueue, void*, void*>(*this, &TaskQueue::worker)))
		{
		}
		TaskQueue::~TaskQueue()
		{
			this->stop();
		}

		void* TaskQueue::worker(void*)
		{
			bool end = false;
			while (!end)
			{
				Task* t = NULL;
				STDUTILS_LOCK(this->workerLock)
				{
					if (this->tasks.size() > 0)
					{
						t = &this->tasks.front();
						this->tasks.pop();
					}
				}
				STDUTILS_RELEASE(this->workerLock);

				if (!t)
					this->semaphore.wait();
				else
					t->Worker(t->Parameter);

				STDUTILS_LOCK(this->workerLock)
				{
					end = !this->running;
				}
				STDUTILS_RELEASE(this->workerLock);
			}

			return NULL;
		}

		void TaskQueue::start()
		{
			STDUTILS_SYNCHRONIZE(this->controlLock);
			STDUTILS_LOCK(this->workerLock)
			{
				if (!this->running)
				{
					this->running = true;
					this->thread.start();
				}
			}
			STDUTILS_RELEASE(this->workerLock);
		}
		void TaskQueue::stop()
		{
			STDUTILS_SYNCHRONIZE(this->controlLock);
			STDUTILS_LOCK(this->workerLock)
			{
				if (this->running)
				{
					this->running = false;

					this->thread.stop();
				}
			}
			STDUTILS_RELEASE(this->workerLock);
			this->thread.waitForTermination();
		}

		void TaskQueue::enqueue(IDelegate<void, Dynamic&>& worker, Dynamic const& parameter)
		{
			STDUTILS_SYNCHRONIZE(this->controlLock);
			STDUTILS_LOCK(this->workerLock)
			{
				this->tasks.push(Task(worker, parameter));

				if (this->running)
					this->semaphore.signal();
			}
			STDUTILS_RELEASE(this->workerLock);
		}
	}
}