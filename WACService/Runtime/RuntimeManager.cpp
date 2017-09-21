#include "RuntimeManager.h"
#include "../Core/Service.h"
#include "../Client/ClientManager.h"
#include "../Scan/ScanManager.h"

void Run(WACService& Service, unsigned int ProcessID, unsigned int ScanInterval)
{
	while (VerifyClient())
	{
		if (RunScan() != THREAT_NONE)
		{
			MessageBox(NULL, L"Wave Anti-Cheat has Detected a Runtime Compromise, Shutting Down", L"Wave Anti-Cheat", MB_ICONERROR | MB_OK);

			KillTarget();

			break;
		}

		Sleep(ScanInterval);
	}

	// Runtime has Completed, Stop the Service
	
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE ServiceAccess = NULL;
	SERVICE_STATUS Status = {};

	ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!ServiceControlManager)
	{
		MessageBox(NULL, L"Wave Anti-Cheat Error", L"Wave Anti-Cheat", MB_ICONERROR | MB_OK);
		
		goto Cleanup;
	}

	ServiceAccess = OpenService(ServiceControlManager, Service.Name, SERVICE_STOP | SERVICE_QUERY_STATUS);
	if (!ServiceAccess)
	{
		MessageBox(NULL, L"Wave Anti-Cheat Error", L"Wave Anti-Cheat", MB_ICONERROR | MB_OK);

		goto Cleanup;
	}

	if (ControlService(ServiceAccess, SERVICE_CONTROL_STOP, &Status))
	{
		Sleep(1000);

		while (QueryServiceStatus(ServiceAccess, &Status))
		{
			if (Status.dwCurrentState == SERVICE_STOP_PENDING)
			{
				Sleep(1000);
			}

			else break;
		}

		if (Status.dwCurrentState != SERVICE_STOPPED)
		{
			MessageBox(NULL, L"Wave Anti-Cheat Error", L"Wave Anti-Cheat", MB_ICONERROR | MB_OK);

			goto Cleanup;
		}
	}

	else
	{
		MessageBox(NULL, L"Wave Anti-Cheat Error", L"Wave Anti-Cheat", MB_ICONERROR | MB_OK);

		goto Cleanup;
	}

Cleanup:
	if (ServiceControlManager)
	{
		CloseServiceHandle(ServiceControlManager);
		ServiceControlManager = NULL;
	}

	if (ServiceAccess)
	{
		CloseServiceHandle(ServiceAccess);
		ServiceAccess = NULL;
	}
}
