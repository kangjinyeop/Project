#include "stdafx.h"
#include "Server_CrashDump.h"

long CCrashDump::_DumpCount = 0;

CCrashDump::CCrashDump()
{
	_invalid_parameter_handler	OldHandler;
	_invalid_parameter_handler	NewHandler;

	NewHandler = myInvalidParameterHandler;

	// CRT 함수에 null 포인터등 넣었을때..
	OldHandler = _set_invalid_parameter_handler(NewHandler);

	_CrtSetReportMode(_CRT_WARN, 0);
	_CrtSetReportMode(_CRT_ERROR, 0);
	_CrtSetReportMode(_CRT_ASSERT, 0);	

	_CrtSetReportHook(_custom_Report_hook);

	_set_purecall_handler(myPurecallHandler);

	SetHandlerDump();
}

CCrashDump::~CCrashDump()
{
}

void CCrashDump::Crash()
{
	int* Pointer = nullptr;
	*Pointer = 0;
}

LONG __stdcall CCrashDump::MyExceptionFilter(PEXCEPTION_POINTERS pExceptionPointer)
{
	long DumpCount = InterlockedIncrement(&_DumpCount);

	TCHAR FileName[MAX_PATH];

	SYSTEMTIME	NowTime;

	GetLocalTime(&NowTime);
	wsprintf(FileName, L"Dump_%d%02d%02d_%02d.%02d.%02d_%d.dmp",
		NowTime.wYear, NowTime.wMonth, NowTime.wDay, NowTime.wHour,
		NowTime.wMinute, NowTime.wSecond, _DumpCount);

	wprintf(L"Create Dump File..\n");

	HANDLE DumpFile = CreateFile(FileName,
		GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE != DumpFile)
	{
		_MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInformation;

		MiniDumpExceptionInformation.ThreadId = GetCurrentThreadId();
		MiniDumpExceptionInformation.ExceptionPointers = pExceptionPointer;
		MiniDumpExceptionInformation.ClientPointers = TRUE;

		MiniDumpWriteDump(GetCurrentProcess(),
						GetCurrentProcessId(),
						DumpFile,
						MiniDumpWithFullMemory,
						&MiniDumpExceptionInformation,
						NULL, NULL);

		CloseHandle(DumpFile);

		wprintf(L"Create Dump File Success");
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void CCrashDump::SetHandlerDump()
{
	SetUnhandledExceptionFilter(MyExceptionFilter);
}

void CCrashDump::myInvalidParameterHandler(const TCHAR * expression, const TCHAR * function, const TCHAR * file, UINT line, uintptr_t pReserved)
{
	Crash();
}

int CCrashDump::_custom_Report_hook(int ireposttype, char * message, int * returnvalue)
{
	Crash();

	return true;
}

void CCrashDump::myPurecallHandler()
{
	Crash();
}