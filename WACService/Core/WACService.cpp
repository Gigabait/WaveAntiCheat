#include <wchar.h>

#include "Service.h"
#include "ServiceManager.h"
#include "../Runtime/RuntimeManager.h"
#include "../Client/ClientManager.h"

#define SERVICE_NAME         L"WACService"
#define SERVICE_DISPLAY_NAME L"Wave Anti-Cheat Service"
#define SERVICE_START_TYPE   SERVICE_DEMAND_START
#define SERVICE_DEPENDENCIES L""
#define SERVICE_ACCOUNT      L"LocalSystem"
#define SERVICE_PASSWORD     NULL

#define SCAN_INTERVAL_MS     5000

#define PRINT_USAGE wprintf(L"Guard Usage:\n"); \
					wprintf(L" -guard -id PID\n"); \
					wprintf(L" -guard -name Name\n\n");

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
			if (ArgCount > 3)
			{
				unsigned int PID;

				if (_wcsicmp(L"id", Args[2] + 1) == 0)
				{
					PID = _wtoi(Args[3]);
				}

				else if (_wcsicmp(L"name", Args[2] + 1) == 0)
				{
					PID = FindProcess(Args[3]);
				}

				else
				{
					PRINT_USAGE

					return 1;
				}

				switch (AttachClient(PID))
				{
				case WACAC_NOERR: break;
				case WACAC_NODLL: MessageBox(NULL, L"Wave Anti-Cheat Failed: Could Not Locate WACClient.dll", L"WAC Fatal Error", MB_OK | MB_ICONERROR); return 1;
				case WACAC_NOPROCESS: MessageBox(NULL, L"Wave Anti-Cheat Failed: Could Not Connect to Target Process", L"WAC Fatal Error", MB_OK | MB_ICONERROR); return 1;
				default: MessageBox(NULL, L"Wave Anti-Cheat Failed: Unknown Error Occurred", L"WAC Fatal Error", MB_OK | MB_ICONERROR); return 1;
				}

				WACService* WAC = new WACService(SERVICE_NAME, TRUE, TRUE, FALSE);

				// Begin Service Message Handler
				WACService::Run(*WAC);

				// Begin WAC Runtime
				Run(*WAC, PID, SCAN_INTERVAL_MS);

				delete WAC;
			}

			else
			{
				PRINT_USAGE

				return 1;
			}
		}
	}

	else
	{
		wprintf(L"Parameters:\n");
		wprintf(L" -install      Installs the service.\n");
		wprintf(L" -uninstall    Uninstalls the service.\n\n");
		wprintf(L" -guard  Activates the service, begins safeguarding the process specified from threats.\n"
				 "               The service will resume idling when the target process closes.\n\n");

		PRINT_USAGE

		return 1;
	}

	return 0;
}