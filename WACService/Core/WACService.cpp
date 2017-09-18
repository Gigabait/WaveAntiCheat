#include <wchar.h>

#include "ServiceManager.h"

#define SERVICE_NAME         L"WACService"
#define SERVICE_DISPLAY_NAME L"Wave Anti-Cheat Service"
#define SERVICE_START_TYPE   SERVICE_DEMAND_START
#define SERVICE_DEPENDENCIES L""
#define SERVICE_ACCOUNT      L"LocalSystem"
#define SERVICE_PASSWORD     NULL

int wmain(int ArgCount, wchar_t* Args[])
{
	if (ArgCount > 1)
	{
		if (_wcsicmp(L"install", Args[1] + 1) == 0)
		{
			InstallWACService(
				SERVICE_NAME,
				SERVICE_DISPLAY_NAME,
				SERVICE_START_TYPE,
				SERVICE_DEPENDENCIES,
				SERVICE_ACCOUNT,
				SERVICE_PASSWORD);
		}

		else if (_wcsicmp(L"uninstall", Args[1] + 1) == 0)
		{
			UninstallWACService(SERVICE_NAME);
		}
	}

	else
	{
		wprintf(L"Parameters:\n");
		wprintf(L" -install    To install the service.\n");
		wprintf(L" -uninstall  To uninstall the service.\n");

		return 1;
	}

	return 0;
}