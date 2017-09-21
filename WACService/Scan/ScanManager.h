#pragma once

enum ScanType
{
	SCAN_ALL,
	SCAN_BLACKLIST,
	SCAN_DEBUGGER,
};

enum ThreatResult
{
	THREAT_NONE,
	THREAT_COMPROMISED,
};

ThreatResult RunScan();
ThreatResult RunScan(ScanType Scan);