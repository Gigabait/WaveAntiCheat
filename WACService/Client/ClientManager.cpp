#include "ClientManager.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
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
	HANDLE DLLHandle = FindFirstFileA(WAC_CLIENT_FILENAME, &FindDLLData);
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

	GetFullPathNameA(WAC_CLIENT_FILENAME, MAX_PATH, AbsPath, NULL);

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

	TargetProcessID = ProcessID;

	return WACAC_NOERR;
}

bool VerifyClient()
{
	HANDLE Target = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, TargetProcessID);
	if (!Target || Target == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	HMODULE ModuleList[1024];
	DWORD cbNeeded;
	if (!EnumProcessModules(Target, ModuleList, sizeof(ModuleList), &cbNeeded))
	{
		CloseHandle(Target);

		return false;
	}

	char ModuleName[MAX_PATH] = { 0 };

	for (int Iter = 0; Iter < (cbNeeded / sizeof(HMODULE)); ++Iter)
	{
		// Use ANSI to Match WAC_CLIENT_FILENAME
		if (!GetModuleFileNameExA(Target, ModuleList[Iter], ModuleName, sizeof(ModuleName) / sizeof(char)))
		{
			CloseHandle(Target);

			return false;
		}

		if (ModuleName == WAC_CLIENT_FILENAME)
		{
			CloseHandle(Target);

			return true;
		}
	}

	CloseHandle(Target);

	return false;
}

void KillTarget()
{
	HANDLE Target = OpenProcess(PROCESS_TERMINATE, FALSE, TargetProcessID);
	if (!Target || Target == INVALID_HANDLE_VALUE)
	{
		// @todo Some Proper Error Handling Here
	}

	TerminateProcess(Target, ERROR_BAD_ENVIRONMENT);

	CloseHandle(Target);
}