#pragma once

#include <Windows.h>

void InstallWACService(PWSTR ServiceName, PWSTR DisplayName, DWORD StartType, PWSTR Dependencies, PWSTR Account, PWSTR Password);
void UninstallWACService(PWSTR ServiceName);