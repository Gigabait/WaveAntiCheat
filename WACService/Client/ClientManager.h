#pragma once

#define WAC_CLIENT_PATHNAME L"WACClient.dll"

// Error Codes
#define WACERR_NOERROR   0
#define WACERR_NODLL     1
#define WACERR_NOPROCESS 2
#define WACERR_UNKNOWN   3

// Utilized WINAPI to Inject WACClient.dll into the Target
int AttachClient(unsigned int ProcessID);