#include "ServiceBase.h"
#include <assert.h>
#include <strsafe.h>

WACServiceBase* WACServiceBase::Service = nullptr;

WACServiceBase::WACServiceBase(PWSTR ServiceName, BOOL CanStop = TRUE, BOOL CanShutdown = TRUE, BOOL CanPauseContinue = FALSE)
{
	Name = (ServiceName == NULL ? L"" : ServiceName);
	StatusHandle = NULL;

	Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	Status.dwCurrentState = SERVICE_START_PENDING;

	DWORD ControlsAccepted = 0;
	ControlsAccepted |= (CanStop ? SERVICE_ACCEPT_STOP : 0);
	ControlsAccepted |= (CanShutdown ? SERVICE_ACCEPT_SHUTDOWN : 0);
	ControlsAccepted |= (CanPauseContinue ? SERVICE_ACCEPT_PAUSE_CONTINUE : 0);

	Status.dwControlsAccepted = ControlsAccepted;
	Status.dwWin32ExitCode = NO_ERROR;
	Status.dwServiceSpecificExitCode = 0;
	Status.dwCheckPoint = 0;
	Status.dwWaitHint = 0;
}

WACServiceBase::~WACServiceBase()
{
}

BOOL WACServiceBase::Run(WACServiceBase& Service)
{
	WACServiceBase::Service = &Service;

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ Service.Name, ServiceMain },
		{ NULL, NULL },
	};

	return StartServiceCtrlDispatcher(ServiceTable);
}

void WINAPI WACServiceBase::ServiceMain(DWORD ArgCount, LPWSTR* Args)
{
	assert(Service);

	Service->StatusHandle = RegisterServiceCtrlHandler(Service->Name, ServiceCtrlHandler);
	if (!Service->StatusHandle)
	{
		throw GetLastError();
	}

	Service->Start(ArgCount, Args);
}

void WINAPI WACServiceBase::ServiceCtrlHandler(DWORD Ctrl)
{
	switch (Ctrl)
	{
	case SERVICE_CONTROL_STOP: Service->Stop();
	case SERVICE_CONTROL_PAUSE: Service->Pause();
	case SERVICE_CONTROL_CONTINUE: Service->Continue();
	case SERVICE_CONTROL_SHUTDOWN: Service->Shutdown();
	case SERVICE_CONTROL_INTERROGATE: break;
	default: break;
	}
}

void WACServiceBase::Start(DWORD ArgCount, PWSTR* Args)
{
	try
	{
		SetServiceStatus(SERVICE_START_PENDING);
		OnStart(ArgCount, Args);
		SetServiceStatus(SERVICE_RUNNING);
	}

	catch (DWORD Error)
	{
		WriteErrorLogEntry(L"Service Start", Error);
		SetServiceStatus(SERVICE_STOPPED, Error);
	}

	catch (...)
	{
		WriteEventLogEntry(L"Service Start Failure", EVENTLOG_ERROR_TYPE);
		SetServiceStatus(SERVICE_STOPPED);
	}
}

void WACServiceBase::Stop()
{
	DWORD OriginalState = Status.dwCurrentState;

	try
	{
		SetServiceStatus(SERVICE_STOP_PENDING);
		OnStop();
		SetServiceStatus(SERVICE_STOPPED);
	}

	catch (DWORD Error)
	{
		WriteErrorLogEntry(L"Service Stop", Error);
		SetServiceStatus(OriginalState);
	}

	catch (...)
	{
		WriteEventLogEntry(L"Service Stop Failure", EVENTLOG_ERROR_TYPE);
		SetServiceStatus(OriginalState);
	}
}

void WACServiceBase::Pause()
{
	try
	{
		SetServiceStatus(SERVICE_PAUSE_PENDING);
		OnPause();
		SetServiceStatus(SERVICE_PAUSED);
	}

	catch (DWORD Error)
	{
		WriteErrorLogEntry(L"Service Pause", Error);
		SetServiceStatus(SERVICE_RUNNING, Error);
	}

	catch (...)
	{
		WriteEventLogEntry(L"Service Pause Failure", EVENTLOG_ERROR_TYPE);
		SetServiceStatus(SERVICE_RUNNING);
	}
}

void WACServiceBase::Continue()
{
	try
	{
		SetServiceStatus(SERVICE_CONTINUE_PENDING);
		OnContinue();
		SetServiceStatus(SERVICE_RUNNING);
	}

	catch (DWORD Error)
	{
		WriteErrorLogEntry(L"Service Continue", Error);
		SetServiceStatus(SERVICE_PAUSED, Error);
	}

	catch (...)
	{
		WriteEventLogEntry(L"Service Continue Failure", EVENTLOG_ERROR_TYPE);
		SetServiceStatus(SERVICE_PAUSED);
	}
}

void WACServiceBase::Shutdown()
{
	try
	{
		OnShutdown();
		SetServiceStatus(SERVICE_STOPPED);
	}

	catch (DWORD Error)
	{
		WriteErrorLogEntry(L"Service Shutdown", Error);
	}

	catch (...)
	{
		WriteEventLogEntry(L"Service Shutdown Failure", EVENTLOG_ERROR_TYPE);
	}
}

void WACServiceBase::SetServiceStatus(DWORD CurrentState, DWORD ExitCode = NO_ERROR, DWORD WaitHint = 0)
{
	static DWORD CheckPoint = 1;

	Status.dwCurrentState = CurrentState;
	Status.dwWin32ExitCode = ExitCode;
	Status.dwWaitHint = WaitHint;
	Status.dwCheckPoint = ((CurrentState == SERVICE_RUNNING) || (CurrentState == SERVICE_STOPPED)) ? 0 : CheckPoint++;

	::SetServiceStatus(StatusHandle, &Status);
}

void WACServiceBase::WriteEventLogEntry(PWSTR Message, WORD Type)
{
	HANDLE EventSource = NULL;
	LPCWSTR Strings[2] = { NULL, NULL };

	EventSource = RegisterEventSource(NULL, Name);
	if (EventSource)
	{
		Strings[0] = Name;
		Strings[1] = Message;

		ReportEvent(EventSource, Type, 0, 0, NULL, 2, 0, Strings, NULL);

		DeregisterEventSource(EventSource);
	}
}

void WACServiceBase::WriteErrorLogEntry(PWSTR Function, DWORD Error = GetLastError())
{
	wchar_t Message[260];

	StringCchPrintf(Message, ARRAYSIZE(Message), L"%s Failed: 0x%08lx", Function, Error);

	WriteEventLogEntry(Message, EVENTLOG_ERROR_TYPE);
}
