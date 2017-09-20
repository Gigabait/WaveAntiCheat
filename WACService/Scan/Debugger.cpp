#include "Debugger.h"

#include <Windows.h>

bool ScanDebuggerInternal()
{
	return IsDebuggerPresent();
}

bool ScanDebuggerExternal(unsigned int ProcessID)
{
	bool Result;
	HANDLE Target = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);

	CheckRemoteDebuggerPresent(Target, (PBOOL)&Result);

	CloseHandle(Target);

	return Result;
}