#ifndef STDUTILS_WINDOWSSERVICE_H
#define STDUTILS_WINDOWSSERVICE_H

#include <windows.h> 

#include <StdUtils/Exceptions.h>
#include <StdUtils/Delegates.h>
#include <StdUtils/Nullable.h>

#include <StdUtils/Application/ServiceSignals.h>
#include <StdUtils/Application/LogFile.h>

namespace StdUtils
{
	namespace Application
	{
		class WindowsService
		{ 
		public: 

			// Register the executable for a service with the Service Control Manager  
			// (SCM). After you call Run(ServiceBase), the SCM issues a Start command,  
			// which results in a call to the OnStart method in the service. This  
			// method blocks until the service has stopped. 
			static void Run(WindowsService &service, Nullable<LogFile>& output, Nullable<LogFile>& error);

			// Service object constructor. The optional parameters (fCanStop,  
			// fCanShutdown and fCanPauseContinue) allow you to specify whether the  
			// service can be stopped, paused and continued, or be notified when  
			// system shutdown occurs. 
			WindowsService(String pszServiceName,
				IDelegate<void, ServiceSignals::Value>& onSignal,  
				BOOL fCanStop = TRUE,  
				BOOL fCanShutdown = FALSE,  
				BOOL fCanPauseContinue = FALSE); 

			// Service object destructor.  
			virtual ~WindowsService(void); 

			// Stop the service. 
			void Stop();
			SERVICE_STATUS const& getStatus() const;

		protected: 

			// When implemented in a derived class, executes when a Start command is  
			// sent to the service by the SCM or when the operating system starts  
			// (for a service that starts automatically). Specifies actions to take  
			// when the service starts. 
			void OnStart(DWORD dwArgc, LPSTR *pszArgv); 

			// When implemented in a derived class, executes when a Stop command is  
			// sent to the service by the SCM. Specifies actions to take when a  
			// service stops running. 
			void OnStop(); 

			// When implemented in a derived class, executes when a Pause command is  
			// sent to the service by the SCM. Specifies actions to take when a  
			// service pauses. 
			//void OnPause(); 

			// When implemented in a derived class, OnContinue runs when a Continue  
			// command is sent to the service by the SCM. Specifies actions to take  
			// when a service resumes normal functioning after being paused. 
			//void OnContinue(); 

			// When implemented in a derived class, executes when the system is  
			// shutting down. Specifies what should occur immediately prior to the  
			// system shutting down. 
			//void OnShutdown(); 

			// Set the service status and report the status to the SCM. 
			void SetServiceStatus(DWORD dwCurrentState,  
				DWORD dwWin32ExitCode = NO_ERROR,  
				DWORD dwWaitHint = 0); 

			// Log a message to the Application event log. 
			//void WriteEventLogEntry(PWSTR pszMessage, WORD wType); 

			// Log an error message to the Application event log. 
			//void WriteErrorLogEntry(PWSTR pszFunction,  
			//	DWORD dwError = GetLastError()); 

		private: 

			// Entry point for the service. It registers the handler function for the  
			// service and starts the service. 
			static void WINAPI ServiceMain(DWORD dwArgc, LPSTR *lpszArgv); 

			// The function is called by the SCM whenever a control code is sent to  
			// the service. 
			static void WINAPI ServiceCtrlHandler(DWORD dwCtrl); 

			// Start the service. 
			void Start(DWORD dwArgc, LPSTR *pszArgv); 

			// Pause the service. 
			//void Pause(); 

			// Resume the service after being paused. 
			//void Continue(); 

			// Execute when the system is shutting down. 
			//void Shutdown(); 

			// The singleton service instance. 
			static WindowsService *s_service; 

			Delegate<void, ServiceSignals::Value> signalHandler;

			// The name of the service 
			LPSTR m_name; 

			// The status of the service 
			SERVICE_STATUS m_status; 

			// The service status handle 
			SERVICE_STATUS_HANDLE m_statusHandle; 
		};

		static void InstallService(char const* pszServiceName,  
			char const* pszDisplayName,  
			DWORD dwStartType, 
			char const* pszDependencies,  
			char const* pszAccount,  
			char const* pszPassword) 
		{ 
			char szPath[MAX_PATH]; 
			SC_HANDLE schSCManager = NULL; 
			SC_HANDLE schService = NULL; 

			try
			{
				if (GetModuleFileNameA(NULL, szPath, ARRAYSIZE(szPath)) == 0) 
				{ 
					throw OSApiException("GetModuleFileName failed", GetLastError()); 
				} 

				// Open the local default service control manager database 
				schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT |  
					SC_MANAGER_CREATE_SERVICE); 
				if (schSCManager == NULL) 
				{ 
					throw OSApiException("OpenSCManager failed", GetLastError()); 
				} 

				// Install the service into SCM by calling CreateService 
				schService = CreateServiceA( 
					schSCManager,                   // SCManager database 
					pszServiceName,                 // Name of service 
					pszDisplayName,                 // Name to display 
					SERVICE_QUERY_STATUS,           // Desired access 
					SERVICE_WIN32_OWN_PROCESS,      // Service type 
					dwStartType,                    // Service start type 
					SERVICE_ERROR_NORMAL,           // Error control type 
					szPath,                         // Service's binary 
					NULL,                           // No load ordering group 
					NULL,                           // No tag identifier 
					pszDependencies,                // Dependencies 
					pszAccount,                     // Service running account 
					pszPassword                     // Password of the account 
					); 
				if (schService == NULL) 
				{ 
					throw OSApiException("CreateService failed", GetLastError()); 
				} 
			}
			catch (...)
			{
				// Centralized cleanup for all allocated resources. 
				if (schSCManager) 
				{ 
					CloseServiceHandle(schSCManager); 
					schSCManager = NULL; 
				} 
				if (schService) 
				{ 
					CloseServiceHandle(schService); 
					schService = NULL; 
				}

				throw;
			}

			if (schSCManager) 
			{ 
				CloseServiceHandle(schSCManager); 
				schSCManager = NULL; 
			} 
			if (schService) 
			{ 
				CloseServiceHandle(schService); 
				schService = NULL; 
			}
		}
		static void UninstallService(char const* pszServiceName) 
		{ 
			SC_HANDLE schSCManager = NULL; 
			SC_HANDLE schService = NULL; 
			SERVICE_STATUS ssSvcStatus = {}; 

			try
			{
				// Open the local default service control manager database 
				schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT); 
				if (schSCManager == NULL) 
				{ 
					throw OSApiException("OpenSCManager failed", GetLastError()); 
				} 

				// Open the service with delete, stop, and query status permissions 
				schService = OpenServiceA(schSCManager, pszServiceName, SERVICE_STOP |  
					SERVICE_QUERY_STATUS | DELETE); 
				if (schService == NULL) 
				{ 
					throw OSApiException("OpenService failed", GetLastError()); 
				} 

				// Try to stop the service 
				if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus)) 
				{ 
					Sleep(1000); 

					while (QueryServiceStatus(schService, &ssSvcStatus)) 
					{ 
						if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING) 
						{ 
							Sleep(1000); 
						} 
						else break; 
					} 

					if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED) 
					{ 
						//wprintf(L"\n%s is stopped.\n", pszServiceName); 
					} 
					else 
					{ 
						//wprintf(L"\n%s failed to stop.\n", pszServiceName); 
					} 
				} 

				// Now remove the service by calling DeleteService. 
				if (!DeleteService(schService)) 
				{ 
					throw OSApiException("DeleteService failed", GetLastError()); 
				}
			}
			catch (...)
			{
				// Centralized cleanup for all allocated resources. 
				if (schSCManager) 
				{ 
					CloseServiceHandle(schSCManager); 
					schSCManager = NULL; 
				} 
				if (schService) 
				{ 
					CloseServiceHandle(schService); 
					schService = NULL; 
				} 
				throw;
			}

			// Centralized cleanup for all allocated resources. 
			if (schSCManager) 
			{ 
				CloseServiceHandle(schSCManager); 
				schSCManager = NULL; 
			} 
			if (schService) 
			{ 
				CloseServiceHandle(schService); 
				schService = NULL; 
			} 
		}
	}
}

#endif