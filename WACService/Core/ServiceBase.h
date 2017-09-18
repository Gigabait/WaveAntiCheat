#pragma once

#include <Windows.h>

// Singleton Base Object for Abstraction of Windows Service Prerequisites.
class WACServiceBase
{
private:
	static WACServiceBase* Service;

	PWSTR Name;

	SERVICE_STATUS Status;
	SERVICE_STATUS_HANDLE StatusHandle;

public:
	WACServiceBase(PWSTR ServiceName, BOOL CanStop = TRUE, BOOL CanShutdown = TRUE, BOOL CanPauseContinue = FALSE);
	virtual ~WACServiceBase();

	static BOOL Run(WACServiceBase& Service);

private:
	static void WINAPI ServiceMain(DWORD ArgCount, LPWSTR* Args);
	static void WINAPI ServiceCtrlHandler(DWORD Ctrl);
	
	void Start(DWORD ArgCount, PWSTR* Args);
	void Stop();
	void Pause();
	void Continue();
	void Shutdown();

protected:
	virtual void OnStart(DWORD ArgCount, PWSTR* Args) {}
	virtual void OnStop() {}
	virtual void OnPause() {}
	virtual void OnContinue() {}
	virtual void OnShutdown() {}

	void SetServiceStatus(DWORD CurrentState, DWORD ExitCode = NO_ERROR, DWORD WaitHint = 0);

	void WriteEventLogEntry(PWSTR Message, WORD Type);
	void WriteErrorLogEntry(PWSTR Function, DWORD Error = GetLastError());
};