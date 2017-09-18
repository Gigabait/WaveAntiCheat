#include "ServiceManager.h"
#include <stdio.h>

void InstallWACService(PWSTR ServiceName, PWSTR DisplayName, DWORD StartType, PWSTR Dependencies, PWSTR Account, PWSTR Password)
{
	wchar_t Path[MAX_PATH];
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE Service = NULL;

	if (GetModuleFileName(NULL, Path, ARRAYSIZE(Path)) == 0)
	{
		wprintf(L"GetMoudleFileName Failed: 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (!ServiceControlManager)
	{
		wprintf(L"OpenSCManager Failed: 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	Service = CreateService(
		ServiceControlManager,
		ServiceName,
		DisplayName,
		SERVICE_QUERY_STATUS,
		SERVICE_WIN32_OWN_PROCESS,
		StartType,
		SERVICE_ERROR_NORMAL,
		Path,
		NULL,
		NULL,
		Dependencies,
		Account,
		Password
	);
	if (!Service)
	{
		wprintf(L"CreateService Failed: 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	wprintf(L"%s Has Been Successfully Installed\n", ServiceName);

Cleanup:
	if (ServiceControlManager)
	{
		CloseServiceHandle(ServiceControlManager);
		ServiceControlManager = NULL;
	}

	if (Service)
	{
		CloseServiceHandle(Service);
		Service = NULL;
	}
}

void UninstallWACService(PWSTR ServiceName)
{
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE Service = NULL;
	SERVICE_STATUS Status = {};

	ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	if (!ServiceControlManager)
	{
		wprintf(L"OpenSCManager Failed: 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	Service = OpenService(ServiceControlManager, ServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
	if (!Service)
	{
		wprintf(L"OpenService Failed: 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	if (ControlService(Service, SERVICE_CONTROL_STOP, &Status))
	{
		wprintf(L"Stopping %s...\n", ServiceName);
		Sleep(1000);

		while (QueryServiceStatus(Service, &Status))
		{
			if (Status.dwCurrentState == SERVICE_STOP_PENDING)
			{
				Sleep(1000);
			}

			else break;
		}

		if (Status.dwCurrentState == SERVICE_STOPPED)
		{
			wprintf(L"%s Has Successfully Stopped\n", ServiceName);
		}

		else
		{
			wprintf(L"%s Could Not Be Stopped\n", ServiceName);
		}
	}

	if (!DeleteService(Service))
	{
		wprintf(L"DeleteService Failed: 0x%08lx\n", GetLastError());
		goto Cleanup;
	}

	wprintf(L"%s Has Been Successfully Uninstalled\n", ServiceName);

Cleanup:
	if (ServiceControlManager)
	{
		CloseServiceHandle(ServiceControlManager);
		ServiceControlManager = NULL;
	}

	if (Service)
	{
		CloseServiceHandle(Service);
		Service = NULL;
	}
}
