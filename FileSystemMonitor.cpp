#include <StdUtils/FileSystem/FileSystemMonitor.h>

#ifdef _WINDOWS
#else
#include <stddef.h>
#include <cstddef>
#include <unistd.h>
#include <sys/inotify.h>
#endif

using namespace std;
using namespace StdUtils::Threading;


namespace StdUtils
{
	namespace FileSystem
	{
		FileSystemMonitor::FileSystemMonitor(Directory const& target, IDelegate<void, Exception*>& callback, bool watchSubfolders)
			:target(target),
			callback(callback),
			watchSubfolders(watchSubfolders),
			worker(DelegateMethod<FileSystemMonitor, void*, void*>(*this, &FileSystemMonitor::doWork)),
			lock(),
#ifdef _WINDOWS
			fsHandle(INVALID_HANDLE_VALUE),
#else
			fsHandle(-1),
#endif
			cancelWork(true)
		{
		}
		FileSystemMonitor::~FileSystemMonitor()
		{
			STDUTILS_LOCK(this->lock)
			{
				this->callback = Delegate<void,Exception*>();
			}
			STDUTILS_RELEASE(this->lock);

			this->stop();
		}

		void FileSystemMonitor::start()
		{
#ifndef _WINDOWS
			if (this->watchSubfolders)
				throw Exception("watchSubfolders is not yet supported for non-windows environments");
#endif
			STDUTILS_LOCK(this->lock)
			{
				if (this->cancelWork)
				{
					this->cancelWork = false;

					if (!this->worker.isRunning())
						this->worker.start();
				}
			}
			STDUTILS_RELEASE(this->lock);
		}
		void FileSystemMonitor::stop()
		{
			bool doWait = false;
			STDUTILS_LOCK(this->lock)
			{
				if (!this->cancelWork)
				{
					doWait = true;
					this->cancelWork = true;

#ifdef _WINDOWS
					if (this->fsHandle != INVALID_HANDLE_VALUE)
					{
						FindCloseChangeNotification(this->fsHandle);
						this->fsHandle = INVALID_HANDLE_VALUE;
					}
#else
					if (this->fsHandle != -1)
					{
						close(this->fsHandle);
						this->fsHandle = -1;
					}
#endif
				}
			}
			STDUTILS_RELEASE(this->lock);

			if (doWait)
				this->worker.waitForTermination();
		}

#ifdef _WINDOWS
		void* FileSystemMonitor::doWork(void*)
		{
			HANDLE changeHandle;

			STDUTILS_LOCK(this->lock)
			{
				if (!this->cancelWork)
				{
					this->fsHandle = FindFirstChangeNotificationA(this->target.getPath().data(), (this->watchSubfolders ? TRUE : FALSE), FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME);
					if (this->fsHandle == INVALID_HANDLE_VALUE)
					{
						OSApiException ex("Could not initialize File monitor handle", GetLastError());
						this->callback(&ex);
						STDUTILS_RELEASE_AND_RETURN(this->lock, NULL);
					}
					else
						changeHandle = this->fsHandle;
				}
				else
					STDUTILS_RELEASE_AND_RETURN(this->lock, NULL);
			}
			STDUTILS_RELEASE(this->lock);

			bool work = true;
			while (work)
			{
				OSApiException ex;
				bool isError = false;

				DWORD result = WaitForSingleObject(changeHandle, INFINITE);
				switch (result)
				{
				case WAIT_ABANDONED:
					//TODO: Sollte nie passieren
					break;
				case WAIT_OBJECT_0:
					break;
				case WAIT_TIMEOUT:
					//TODO: Sollte nie passieren
					break;
				case WAIT_FAILED:
					ex = OSApiException("Error waiting for directory change", GetLastError());
					isError = true;
					break;
				}

				STDUTILS_LOCK(this->lock)
				{
					if (isError)
					{
						FindCloseChangeNotification(changeHandle);
						this->fsHandle = INVALID_HANDLE_VALUE;

						this->callback(&ex);
						STDUTILS_RELEASE_AND_RETURN(this->lock, NULL);
					}
					else
					{
						this->callback(NULL);
						work = !this->cancelWork;
						if (work)
							FindNextChangeNotification(changeHandle);
					}
				}
				STDUTILS_RELEASE(this->lock);
			}
			return NULL;
		}
#else
		void* FileSystemMonitor::doWork(void*)
		{
			int watchDescriptor = -1;
			int fileDescriptor = -1;

			STDUTILS_LOCK(this->lock)
			{
				if (!this->cancelWork)
				{
					if (this->fsHandle == -1)
						this->fsHandle = inotify_init();
					if (this->fsHandle != -1)
					{
						fileDescriptor = this->fsHandle;
						String targetFullPath = this->target.getFullPath();
						watchDescriptor = inotify_add_watch(this->fsHandle, targetFullPath.data(),
							IN_CREATE | IN_MOVED_TO | IN_MODIFY);
						if (watchDescriptor == -1)
						{
							OSApiException ex("Unable to set up inotify for folder", errno);
							this->callback(&ex);
						}
					}
					else
					{
						OSApiException ex("Unable to initialize inotifx", errno);
						this->callback(&ex);
					}
				}
			}
			STDUTILS_RELEASE(this->lock);

			struct inotify_event buf[8];
			while (watchDescriptor != -1)
			{
				ssize_t length = read(fileDescriptor, buf, sizeof(buf));

				bool isError = false;
				OSApiException ex;

				if (length > 0)
				{
					//Noch nicht noetig, solange ich nur Events werfe ohne Details, was genau passiert ist
					//					int i = 0;
					//					while (length > 0)
					//					{
					//						int pos = i * sizeof(struct inotify_event);
					//						struct inotify_event* event = (struct inotify_event*)&buffer[pos];
					//
					//
					//
					//						i++;
					//						length -= sizeof(struct inotify_event);
					//					}
				}
				else if (length == 0)
				{
					ex = OSApiException("EOF reached on inotify");
					isError = true;
				}
				else
				{
					ex = OSApiException("Error waiting on inotify", errno);
					isError = true;
				}


				STDUTILS_LOCK(this->lock)
				{
					bool doClose = false;
					if (!isError)
					{
						this->callback(NULL);
						if (this->cancelWork)
							doClose = true;
					}
					else
					{
						doClose = true;
						this->callback(&ex);
					}

					if (doClose)
					{
						int rv = inotify_rm_watch(this->fsHandle, watchDescriptor);
						watchDescriptor = -1;
						if (rv)
						{
							//TODO: Was tun? watch konnte nicht entfernt werden...
						}
					}
				}
				STDUTILS_RELEASE(this->lock);
			}

			return NULL;
		}
#endif

		Directory const& FileSystemMonitor::getTarget() const
		{
			return this->target;
		}
	}
}
