#pragma once

#include "../Core/Service.h"

enum PrecheckResult
{
	PR_CLEAN,
	PR_DRIVERSIGNING,
};

// Ensure Secure Environment Before Running
PrecheckResult RuntimePrechecks();

// Enter Runtime
void Run(WACService& Service, unsigned int ProcessID, unsigned int ScanInterval);