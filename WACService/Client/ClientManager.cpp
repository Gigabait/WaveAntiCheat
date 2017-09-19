#include "ClientManager.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <assert.h>

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

	// ANSI Version of WAC Client Pathname
	char* WAC_CLIENT_PATHNAME_A = new char[ARRAYSIZE(WAC_CLIENT_PATHNAME) * 2 + 1];
	ZeroMemory(WAC_CLIENT_PATHNAME_A, ARRAYSIZE(WAC_CLIENT_PATHNAME) * 2 + 1);
	
	size_t WAC_CLIENT_PATHNAME_A_LENGTH = 0;
	wcstombs_s(&WAC_CLIENT_PATHNAME_A_LENGTH, WAC_CLIENT_PATHNAME_A, ARRAYSIZE(WAC_CLIENT_PATHNAME) * 2, WAC_CLIENT_PATHNAME, (ARRAYSIZE(WAC_CLIENT_PATHNAME) * 2));
	assert(WAC_CLIENT_PATHNAME_A_LENGTH > 0);

	// Load with ANSI
	LPVOID LoadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	LPVOID DLLPathPayload = VirtualAllocEx(Target, NULL, WAC_CLIENT_PATHNAME_A_LENGTH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	WriteProcessMemory(Target, DLLPathPayload, WAC_CLIENT_PATHNAME_A, WAC_CLIENT_PATHNAME_A_LENGTH, NULL);

	HANDLE TargetLoaderThread = CreateRemoteThread(Target, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, DLLPathPayload, 0, NULL);

	int Error = WaitForSingleObject(TargetLoaderThread, INFINITE);
	if (Error != 0)
	{
		CloseHandle(Target);
		delete WAC_CLIENT_PATHNAME_A;

		return WACAC_UNKNOWN;
	}

	VirtualFreeEx(Target, DLLPathPayload, WAC_CLIENT_PATHNAME_A_LENGTH, MEM_RELEASE);
	CloseHandle(TargetLoaderThread);
	CloseHandle(Target);
	delete WAC_CLIENT_PATHNAME_A;

	return WACAC_NOERR;
}
