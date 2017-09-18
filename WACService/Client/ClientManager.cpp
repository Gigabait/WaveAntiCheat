#include "ClientManager.h"

#include <Windows.h>
#include <TlHelp32.h>

int FindProcess(std::wstring ProcessName)
{
	if (ProcessName.length() == 0)
	{
		return 0;
	}

	PROCESSENTRY32 Entry;
	Entry.dwSize = sizeof(Entry);

	HANDLE Flash = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(Flash, &Entry))
	{
		do 
		{
			if (wcscmp(Entry.szExeFile, ProcessName.c_str()) == 0)
			{
				return Entry.th32ProcessID;
			}
		} while (Process32Next(Flash, &Entry));
	}

	return 0;
}

int AttachClient(unsigned int ProcessID)
{
	WIN32_FIND_DATA FindDLLData;
	HANDLE DLLHandle = FindFirstFile(WAC_CLIENT_PATHNAME, &FindDLLData);
	if (DLLHandle == INVALID_HANDLE_VALUE)
	{
		FindClose(DLLHandle);

		return WACAC_NODLL;
	}

	// DLL Exists, Close the Handle
	FindClose(DLLHandle);

	HANDLE Target = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
	if (!Target || Target == INVALID_HANDLE_VALUE)
	{
		return WACAC_NOPROCESS;
	}

	// Begin Injection

	LPVOID LoadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibrary");
	LPVOID DLLPathPayload = VirtualAllocEx(Target, NULL, ARRAYSIZE(WAC_CLIENT_PATHNAME), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	WriteProcessMemory(Target, DLLPathPayload, WAC_CLIENT_PATHNAME, ARRAYSIZE(WAC_CLIENT_PATHNAME), NULL);

	HANDLE TargetLoaderThread = CreateRemoteThread(Target, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, DLLPathPayload, 0, NULL);

	int Error = WaitForSingleObject(TargetLoaderThread, INFINITE);
	if (Error != 0)
	{
		CloseHandle(Target);

		return WACAC_UNKNOWN;
	}

	VirtualFreeEx(Target, DLLPathPayload, ARRAYSIZE(WAC_CLIENT_PATHNAME), MEM_RELEASE);
	CloseHandle(TargetLoaderThread);
	CloseHandle(Target);

	return WACAC_NOERR;
}
