#include <wchar.h>

#include "ServiceManager.h"

#define SERVICE_NAME         L"WACService"
#define SERVICE_DISPLAY_NAME L"Wave Anti-Cheat Service"
#define SERVICE_START_TYPE   SERVICE_DEMAND_START
#define SERVICE_DEPENDENCIES L""
#define SERVICE_ACCOUNT      L"NT AUTHORITY\\LocalService"
#define SERVICE_PASSWORD     NULL 

int wmain(int ArgCount, wchar_t* Args[])
{
	if (ArgCount > 1)
	{
		if (_wcsicmp(L"install", Args[1] + 1) == 0)
		{
			
		}

		else if (_wcsicmp(L"uninstall", Args[1] + 1) == 0)
		{

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