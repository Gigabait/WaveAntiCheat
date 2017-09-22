#include "SystemChecks.h"

#include <Windows.h>

bool DriverSigning()
{ 
	HKEY DSPolicy;

	if (!RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Driver Signing", 0, KEY_QUERY_VALUE, &DSPolicy))
	{
		return false;
	}

	DWORD Type = REG_BINARY;
	BYTE Result;
	DWORD Size = sizeof(BYTE);

	if (!RegQueryValueEx(DSPolicy, L"Policy", NULL, &Type, &Result, &Size))
	{
		RegCloseKey(DSPolicy);

		return false;
	}

	// Only Accept Unsigned Blocking
	if (Result != 0x02)
	{
		RegCloseKey(DSPolicy);

		return false;
	}

	RegCloseKey(DSPolicy);

	return true;
}