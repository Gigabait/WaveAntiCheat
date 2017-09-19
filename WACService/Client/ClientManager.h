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

// Utilize WINAPI to Inject the WAC CLient into the Target Process
int AttachClient(unsigned int ProcessID);
