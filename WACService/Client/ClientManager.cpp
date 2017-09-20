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
	WIN32_FIND_DATAA FindDLLData;
	HANDLE DLLHandle = FindFirstFileA(WAC_CLIENT_PATHNAME, &FindDLLData);
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

	char* AbsPath = new char[MAX_PATH];

	GetFullPathNameA(WAC_CLIENT_PATHNAME, MAX_PATH, AbsPath, NULL);

	std::string AbsPathStr = AbsPath;

	LPVOID LoadLibraryAddress = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	LPVOID DLLPathPayload = VirtualAllocEx(Target, NULL, AbsPathStr.length(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	WriteProcessMemory(Target, DLLPathPayload, AbsPathStr.c_str(), AbsPathStr.length(), NULL);

	HANDLE TargetLoaderThread = CreateRemoteThread(Target, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryAddress, DLLPathPayload, 0, NULL);

	int Error = WaitForSingleObject(TargetLoaderThread, INFINITE);
	if (Error != 0)
	{
		CloseHandle(Target);
		delete AbsPath;

		return WACAC_UNKNOWN;
	}

	VirtualFreeEx(Target, DLLPathPayload, AbsPathStr.length(), MEM_RELEASE);
	CloseHandle(TargetLoaderThread);
	CloseHandle(Target);
	delete AbsPath;

	return WACAC_NOERR;
}
