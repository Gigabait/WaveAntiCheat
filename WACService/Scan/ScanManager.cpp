#include "ScanManager.h"

#include "Blacklist.h"
#include "Debugger.h"

// For TargetProcessID
#include "../Client/ClientManager.h"

ThreatResult RunScan()
{
	for (int Iter = SCAN_ALL; Iter != SCAN_DEBUGGER; ++Iter)
	{
		if (RunScan(static_cast<ScanType>(Iter)))
		{
			return THREAT_COMPROMISED;
		}
	}

	return THREAT_NONE;
}

ThreatResult RunScan(ScanType Scan)
{
	bool Result = false;

	switch (Scan)
	{
	case SCAN_BLACKLIST:
		Result = ScanBlacklist();
		if (Result) return THREAT_COMPROMISED;

	case SCAN_DEBUGGER:
		Result = ScanDebuggerInternal();
		if (Result) return THREAT_COMPROMISED;
		Result = ScanDebuggerExternal(TargetProcessID);
		if (Result) return THREAT_COMPROMISED;
	}

	return THREAT_NONE;
}