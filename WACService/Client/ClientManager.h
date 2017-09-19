#pragma once

#include <string>

// Relative or Absolute
#define WAC_CLIENT_PATHNAME L"WACClient.dll"

// AttachClient() Return Codes
#define WACAC_NOERR     1
#define WACAC_NODLL     2
#define WACAC_NOPROCESS 3
#define WACAC_UNKNOWN   4

int FindProcess(std::wstring ProcessName);

// Utilized WINAPI to Inject WACClient.dll into the Target
int AttachClient(unsigned int ProcessID);
