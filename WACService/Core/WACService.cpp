#include <wchar.h>

#include "Service.h"
#include "ServiceManager.h"
#include "../Runtime/RuntimeManager.h"

#define SERVICE_NAME         L"WACService"
#define SERVICE_DISPLAY_NAME L"Wave Anti-Cheat Service"
#define SERVICE_START_TYPE   SERVICE_DEMAND_START
#define SERVICE_DEPENDENCIES L""
#define SERVICE_ACCOUNT      L"LocalSystem"
#define SERVICE_PASSWORD     NULL

#define SCAN_INTERVAL_MS     5000

int wmain(int ArgCount, wchar_t* Args[])
{
	if (ArgCount > 1)
	{
		if (_wcsicmp(L"install", Args[1] + 1) == 0)
		{
			InstallWACService(SERVICE_NAME, SERVICE_DISPLAY_NAME, SERVICE_START_TYPE, SERVICE_DEPENDENCIES, SERVICE_ACCOUNT, SERVICE_PASSWORD);
		}

		else if (_wcsicmp(L"uninstall", Args[1] + 1) == 0)
		{
			UninstallWACService(SERVICE_NAME);
		}

		else if (_wcsicmp(L"guard", Args[1] + 1) == 0)
		{
			if (ArgCount == 3)
			{
				WACService* WAC = new WACService(SERVICE_NAME, TRUE, TRUE, FALSE);

				// Begin Service Message Handler
				WACService::Run(*WAC);

				// Begin WAC Runtime
				Run(*WAC, SCAN_INTERVAL_MS);

				delete WAC;
			}

			else
			{
				wprintf(L"Usage: -guard PID\n");

				return 1;
			}
		}
	}

	else
	{
		wprintf(L"Parameters:\n");
		wprintf(L" -install    Installs the service.\n");
		wprintf(L" -uninstall  Uninstalls the service.\n\n");
		wprintf(L" -guard PID  Activates the service, begins safeguarding the process specified from threats.\n"
				 "             The service will resume idling when the target process closes.\n\n");

		return 1;
	}

	return 0;
}