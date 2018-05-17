#ifndef STDUTILS_SERVICE_H
#define STDUTILS_SERVICE_H


#include <StdUtils/Base.h>
#include <StdUtils/Delegates.h>
#include <StdUtils/Nullable.h>

#include <StdUtils/Application/LogFile.h>
#include <StdUtils/Application/ServiceException.h>
#include <StdUtils/Application/ServiceSignals.h>

#include <vector>


namespace StdUtils
{
	namespace Application
	{
		class Service
		{
			STDUTILS_PREVENT_COPY(Service);
			STDUTILS_PREVENT_ASSIGNMENT(Service);

		public:
			STDUTILS_START_ENUM(LinuxSwitchActions)
			{
				Start,
					Restart,
					Stop
			}
			STDUTILS_END_ENUM(LinuxSwitchActions);
			STDUTILS_START_ENUM(WindowsSwitchActions)
			{
				Install,
					Uninstall
			}
			STDUTILS_END_ENUM(WindowsSwitchActions);
		private:
			struct Switch
			{
				String Argument;
				int Action;
				IDelegate<bool, int>* Handler;
			};


		private:
			String name;
			std::vector<Switch> switches;
			Nullable<LogFile> outputFile;
			Nullable<LogFile> errorFile;
			
#ifdef _WINDOWS
			//String displayName;
			int startType;
			String dependencies;
			String account;
			String password;
#endif

			bool running;

		private:
			Service();

		public:
			~Service();

		public:
			void setName(String const& name);
			String const& getName() const;

			void addSwitch(String const& argument, int action, IDelegate<bool, int>& switchHandler);
			void addLinuxSwitch(String const& argument, LinuxSwitchActions::Value action, IDelegate<bool, int>& switchHandler);
			void addLinuxSwitch(String const& argument, LinuxSwitchActions::Value action);
			void addWindowsSwitch(String const& argument, WindowsSwitchActions::Value action, IDelegate<bool, int>& switchHandler);
			void addWindowsSwitch(String const& argument, WindowsSwitchActions::Value action);

			void addDefaultSwitches();

			void setOutputFile(String const& filepath/*, bool append*/);
			void setErrorFile(String const& filepath/*, bool append*/);

			//TODO?: Ich sollte eventuelll dafuer sorgen, dass der Service eine eigene main-Funktion implementiert
			//Diese wird nur kompiliert, wenn STDUTILS_SERVICE oder so definiert ist.
			//Sie ruft eine Callback-Funktion auf, die genutzt werden kann um den Service zu konfigurieren (Name, Switches, Logfiles, ...)
			//Damit wuerde eindeutiger werden, wie diese Klasse genutzt wird. handleCall ist sehr kryptisch und wenn ich ne Zeit lang nichts damit gemacht hab werde ich mir immer wieder die Implementierung angucken muessen bevor ich es nutzen kann
			int handleCall(int argc, char** argv, IDelegate<void, ServiceSignals::Value>& serviceMethod);

#ifdef _WINDOWS
			//void setDisplayName(String const& displayName);
			void setStartType(int startType);
			void setDependencies(String const& dependencies);
			void setAccount(String const& account);
			void setPassword(String const& password);

			//String const& getDisplayName() const;
			int getStartType() const;
			String const& getDependencies() const;
			String const& getAccount() const;
			String const& getPassword() const;
#endif

		public:
			static Service& Instance()
			{
				static Service s;
				return s;
			}
			static int HandleCall(int argc, char** argv, IDelegate<void, ServiceSignals::Value>& serviceMethod)
			{
				return Service::Instance().handleCall(argc, argv, serviceMethod);
			}
		};
	}
}


#endif