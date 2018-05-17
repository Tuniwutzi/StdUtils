#include <StdUtils/Application/Service.h>


#include <StdUtils/FileSystem.h>
#include <StdUtils/Exceptions.h>


#ifdef _WINDOWS
#include <private/Application/WindowsService.h>
#else
#include <private/Application/LinuxDaemon.h>
#endif


using namespace StdUtils::FileSystem;

using namespace std;

namespace StdUtils
{
	namespace Application
	{
		static String getExecutablePath()
		{
#ifdef _WINDOWS
			char buf[MAX_PATH + 1];
			if (GetModuleFileNameA(NULL, buf, sizeof(buf)) == 0)
				throw OSApiException("GetModuleFileName failed", GetLastError());

			return File(buf).getPath();
#else
			return "";
#endif
		}
		Service::Service()
			:running(false),
			name(Path::GetFileName(getExecutablePath(), false))
#ifdef _WINDOWS
			,
			//displayName(name),
			startType(SERVICE_DEMAND_START),
			dependencies(""),
			account("NT AUTHORITY\\LocalService"),
			password("")
#endif
		{
		}
		Service::~Service()
		{
		}

		void Service::setName(String const& name)
		{
			this->name = name;
			//this->displayName = name;
		}

		void Service::addSwitch(String const& argument, int action, IDelegate<bool, int>& h)
		{
			IDelegate<bool, int>* handler = h.isNull() ? NULL : h.copy();
			try
			{
				if (argument.find(' ') != argument.npos)
					throw Exception("Switch argument must not contain space");

				Switch s;
				s.Argument = argument;
				s.Action = action;
				s.Handler = handler;
				this->switches.push_back(s);
			}
			catch (...)
			{
				delete handler;
				throw;
			}
		}
#ifdef _WINDOWS
		void Service::addLinuxSwitch(String const&, Service::LinuxSwitchActions::Value)
		{
		}
		void Service::addLinuxSwitch(String const&, Service::LinuxSwitchActions::Value, IDelegate<bool, int>&)
		{
		}
		void Service::addWindowsSwitch(String const& arg, Service::WindowsSwitchActions::Value action)
		{
			this->addSwitch(arg, action, Delegate<bool, int>());
		}
		void Service::addWindowsSwitch(String const& arg, Service::WindowsSwitchActions::Value action, IDelegate<bool, int>& handler)
		{
			this->addSwitch(arg, action, handler);
		}
#else
		void Service::addLinuxSwitch(String const& arg, Service::LinuxSwitchActions::Value action)
		{
			Delegate<bool, int> dlg;
			this->addSwitch(arg, (int)action, dlg);
		}
		void Service::addLinuxSwitch(String const& arg, Service::LinuxSwitchActions::Value action, IDelegate<bool, int>& handler)
		{
			this->addSwitch(arg, action, handler);
		}
		void Service::addWindowsSwitch(String const&, Service::WindowsSwitchActions::Value)
		{
		}
		void Service::addWindowsSwitch(String const&, Service::WindowsSwitchActions::Value, IDelegate<bool, int>&)
		{
		}
#endif

		void Service::addDefaultSwitches()
		{
			this->addLinuxSwitch("start", LinuxSwitchActions::Start);
			this->addLinuxSwitch("stop", LinuxSwitchActions::Stop);
			this->addLinuxSwitch("restart", LinuxSwitchActions::Restart);

			this->addWindowsSwitch("/install", WindowsSwitchActions::Install);
			this->addWindowsSwitch("-install", WindowsSwitchActions::Install);
			this->addWindowsSwitch("/uninstall", WindowsSwitchActions::Uninstall);
			this->addWindowsSwitch("-uninstall", WindowsSwitchActions::Uninstall);
		}

		void Service::setOutputFile(String const& filepath)
		{
			this->outputFile = LogFile();
			this->outputFile->Path = filepath;
		}
		void Service::setErrorFile(String const& filepath)
		{
			this->errorFile = LogFile();
			this->errorFile->Path = filepath;
		}


		int Service::handleCall(int argc, char** argv, IDelegate<void, ServiceSignals::Value>& serviceMethod)
		{
			if (this->name.size() <= 0)
				throw Exception("Service name must not be empty");

			if (!this->running)
			{
				this->running = true;

				vector<String> args;
				args.reserve(argc);

				for (int i = 1; i < argc; i++)
					args.push_back(argv[i]);

				if (args.size() == 1)
				{
					for (vector<Switch>::iterator it = this->switches.begin(); it != this->switches.end(); it++)
					{
						if (it->Argument == args[0])
						{
							if (!it->Handler || !it->Handler->operator()(it->Action))
							{
								switch (it->Action)
								{
#ifdef _WINDOWS
								case WindowsSwitchActions::Install:
									InstallService(this->name.data(), this->name.data(), this->startType, this->dependencies.data(), this->account.data(), (this->password.empty() ? NULL : this->password.data()));
									running = false;
									break;
								case WindowsSwitchActions::Uninstall:
									UninstallService(this->name.data());
									running = false;
									break;
#else
								case LinuxSwitchActions::Start:
									StartDaemon(this->name, serviceMethod, this->outputFile, this->errorFile);
									running = false;
									break;
								case LinuxSwitchActions::Stop:
									StopDaemon(this->name);
									running = false;
									break;
								case LinuxSwitchActions::Restart:
									RestartDaemon(this->name, serviceMethod, this->outputFile, this->errorFile);
									running = false;
									break;
#endif
								default:
									throw ServiceException(String("Switch '") + it->Argument + "' not handled by custom handler, no default handler available", ServiceException::ServiceExceptionTypes::CallError);
								}
							}
							break;
						}
					}
				}

				if (running == true)
				{
#ifdef _WINDOWS
					WindowsService ws(this->name, serviceMethod);
					WindowsService::Run(ws, this->outputFile, this->errorFile);
					if (ws.getStatus().dwCurrentState == SERVICE_START_PENDING)
						throw ServiceException("The service was not installed"); //Wahrscheinlich
					return ws.getStatus().dwWin32ExitCode;
#else
					//Bei einem Daemon wird erwartet, dass er nur mit start-argument gestartet werden kann - also in diesem Fall Exception werfen, das Programm kann ggf. Hilfsmessage ausgeben
					throw ServiceException("No argument supplied", ServiceException::ServiceExceptionTypes::ArgumentError);
#endif
				}
				else
					return 0;
			}
			else
				throw ServiceException("Service is already running");
		}
	}
}
