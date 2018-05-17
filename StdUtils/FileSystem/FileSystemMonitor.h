#ifndef STDUTILS_FILESYSTEMMONITOR_H
#define STDUTILS_FILESYSTEMMONITOR_H

#include <StdUtils/Delegates.h>
#include <StdUtils/FileSystem/Directory.h>
#include <StdUtils/Threading/Thread.h>
#include <StdUtils/Threading/Mutex.h>

namespace StdUtils
{
	namespace FileSystem
	{
		class FileSystemMonitor
		{
			STDUTILS_PREVENT_ASSIGNMENT(FileSystemMonitor);
			STDUTILS_PREVENT_COPY(FileSystemMonitor);

		private:
			Directory target;
			Delegate<void, Exception*> callback;
			bool watchSubfolders;

			Threading::Thread worker;
			Threading::Mutex lock;
#ifdef _WINDOWS
			HANDLE fsHandle;
#else
			int fsHandle;
#endif
			bool cancelWork;

		public:
			explicit FileSystemMonitor(Directory const& target, IDelegate<void, Exception*>& callback, bool watchSubfolders = false);
			~FileSystemMonitor();

		private:
			void* doWork(void*);

		public:
			void start();
			void stop();

			Directory const& getTarget() const;
			bool isWatchingSubfolders() const;
		};
	}
}

#endif