#pragma once

#include <string>

// ANSI File Name
#define WAC_CLIENT_FILENAME "WACClient.dll"

// AttachClient() Return Codes
#define WACAC_NOERR     1
#define WACAC_NODLL     2
#define WACAC_NOPROCESS 3
#define WACAC_UNKNOWN   4

// Global Target Process ID, if Any
static unsigned int TargetProcessID;

int FindProcess(std::wstring ProcessName);

// Utilize WINAPI to Inject the WAC CLient into the Target Process
int AttachClient(unsigned int ProcessID);

// Confirm the Client is Still Loaded and Responsive
bool VerifyClient();

// Kill the Target
void KillTarget();