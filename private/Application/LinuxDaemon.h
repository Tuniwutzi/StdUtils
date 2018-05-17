#ifndef STDUTILS_LINUXDAEMON_H
#define STDUTILS_LINUXDAEMON_H

#include <StdUtils/Delegates.h>
#include <StdUtils/Nullable.h>

#include <StdUtils/Application/ServiceSignals.h>
#include <StdUtils/Application/LogFile.h>

namespace StdUtils
{
	namespace Application
	{
		void StartDaemon(String const& name, IDelegate<void, ServiceSignals::Value>& serviceMethod, Nullable<LogFile>& output, Nullable<LogFile>& error);
		void StopDaemon(String const& name);
		void RestartDaemon(String const& name, IDelegate<void, ServiceSignals::Value>& serviceMethod, Nullable<LogFile>& output, Nullable<LogFile>& error);
	}
}

#endif
