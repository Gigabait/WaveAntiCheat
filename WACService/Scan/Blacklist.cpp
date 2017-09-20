#include "Blacklist.h"

#include <Windows.h>

const wchar_t* ProcessBlacklist[] =
{
	L"WinDbgFrameClass",
	L"OLLYDBG",
};

const wchar_t* FileBlacklist[] =
{
	L"CEHYPERSCANSETTINGS",
};

bool BlacklistScan()
{
	for (auto& Process : ProcessBlacklist)
	{
		if (FindWindow((LPCWSTR)Process, NULL))
		{
			return true;
		}
	}

	for (auto& File : FileBlacklist)
	{
		if (OpenFileMapping(FILE_MAP_READ, false, (LPCWSTR)File))
		{
			return true;
		}
	}
}