#include <StdUtils/Base.h>

#ifndef _WINDOWS
#include <private/Application/LinuxDaemon.h>

#include <StdUtils/Exceptions.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

namespace StdUtils
{
	namespace Application
	{
		static String getSemName(String const& fileName)
		{
			return (String("/") + fileName);
		}
		void StartDaemon(String const& name, IDelegate<void, ServiceSignals::Value>& serviceMethod, Nullable<LogFile>& output, Nullable<LogFile>& error)
		{
			String semname(getSemName(name));
			sem_t* sem = sem_open(semname.data(), O_CREAT | O_EXCL, S_IWUSR | S_IRUSR | S_IWOTH | S_IROTH, 0);
			if (sem == SEM_FAILED) //O_EXCL makes sem_open return an error, when this named semaphore already exists
				throw OSApiException("Named semaphore could not be created. This daemon is likely already started", errno);
			else if (sem_close(sem))
				throw OSApiException("Could not close created semaphore", errno);


			pid_t pid = fork();

			if (pid < 0)
				throw OSApiException("Fork unsuccessfull", errno); //Fehler
			else if (pid > 0)
				return; //Wir sind im parent-prozess

			//weiter im daemon

			umask(0);

			pid_t sid = setsid();

			if (sid < 0)
				throw OSApiException("Fork unsuccessfull", errno); //Fehler

			if ((chdir("/")) < 0)
				throw OSApiException("Fork unsuccessfull", errno); //Fehler

			//std-streams schliessen
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			close(STDERR_FILENO);

			//std-streams in /dev/null oeffnen
			open("/dev/null", O_RDONLY); //in
			open(output.hasValue() ? output->Path.data() : "/dev/null", O_RDWR); //out ?
			open(error.hasValue() ? error->Path.data() : "/dev/null", O_RDWR); //err ?

			serviceMethod(ServiceSignals::Start);
			sem = sem_open(semname.data(), 0);
			if (sem != SEM_FAILED)
			{
				//TODO: Wie fehler behandeln?
				sem_wait(sem);
				sem_close(sem);
			}
			serviceMethod(ServiceSignals::Stop);
		}
		void StopDaemon(String const& name)
		{
			String semname(getSemName(name));
			sem_t* sem = sem_open(semname.data(), 0);
			if (sem != SEM_FAILED)
			{
				if (sem_post(sem))
					throw OSApiException("Could not wake daemon process", errno);
				if (sem_close(sem))
					throw OSApiException("Could not close semaphore", errno);
				if (sem_unlink(semname.data()))
					throw OSApiException("Could not unlink semaphore", errno);
			}
			else
				throw OSApiException("Named semaphore could not be opened. The daemon is probably not running", errno);
		}
		void RestartDaemon(String const& name, IDelegate<void, ServiceSignals::Value>& serviceMethod, Nullable<LogFile>& output, Nullable<LogFile>& error)
		{
			StopDaemon(name);
			StartDaemon(name, serviceMethod, output, error);
		}
	}
}
#endif
