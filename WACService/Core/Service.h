#pragma once

#include <Windows.h>

// Singleton Service Object
class WACService
{
private:
	static WACService* Service;

	PWSTR Name;

	SERVICE_STATUS Status;
	SERVICE_STATUS_HANDLE StatusHandle;

public:
	WACService(PWSTR ServiceName, BOOL CanStop = TRUE, BOOL CanShutdown = TRUE, BOOL CanPauseContinue = FALSE);
	virtual ~WACService();

	static BOOL Run(WACService& Service);

private:
	static void WINAPI ServiceMain(DWORD ArgCount, LPWSTR* Args);
	static void WINAPI ServiceCtrlHandler(DWORD Ctrl);
	
// WINAPI Handlers
private:
	void Start(DWORD ArgCount, PWSTR* Args);
	void Stop();
	void Pause();
	void Continue();
	void Shutdown();

// WAC Specific Handlers
private:
	void WACStart(DWORD ArgCount, PWSTR* Args);
	void WACStop();
	void WACPause();
	void WACContinue();
	void WACShutdown();

// Utilities
private:
	void SetServiceStatus(DWORD CurrentState, DWORD ExitCode = NO_ERROR, DWORD WaitHint = 0);

	void WriteEventLogEntry(PWSTR Message, WORD Type);
	void WriteErrorLogEntry(PWSTR Function, DWORD Error = GetLastError());
};