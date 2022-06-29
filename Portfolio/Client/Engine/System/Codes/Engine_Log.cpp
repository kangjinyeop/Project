#include "stdafx.h"
#include "Engine_Log.h"

#include "Engine_Log.inl"

USING(Engine)

IMPLEMENT_SINGLETON(CLog)

CLog::CLog()
	:	_LogCount(0), _FileCreateLimit(MAX_CREATECOUNT), _LogLevel(LOG_LEVEL_END)
{
	InitializeSRWLock(&_SRWLock);
	memset(&_StructTime, 0, sizeof(_StructTime));
}

CLog::~CLog()
{
	Release();
}

void CLog::CreateLogFile(LOG_LEVEL LogLevel, LOG_MODE LogMode, const TCHAR* FileName, const TCHAR* Format, ...)
{
	// 로그 레벨 확인
	if (LogLevel > _LogLevel)
	{
		return;
	}

	AcquireSRWLockExclusive(&_SRWLock);

	// 경로 탐색 및 생성
	CString Directory;

	MakeDirectory(Directory);

	time_t	LocalTime;
	time(&LocalTime);
	localtime_s(&_StructTime, &LocalTime);
	_StructTime.tm_year += 1900;
	_StructTime.tm_mon += 1;

	MakeDirectoryLogTime(Directory);
	MakeDirectoryLogLevel(Directory, LogLevel);

	Directory += L"/";
	Directory += FileName;

	// 파일 읽기
	FILE*	FilePointer = nullptr;

	TCHAR FileMode[4] = L"a+t";

	if (LOG_MODE_BINARY == LogMode)
	{
		FileMode[2] = L'b';
	}

	_wfopen_s(&FilePointer, Directory, FileMode);

	if (nullptr == FilePointer)
	{
		for (int Index = 0; Index < _FileCreateLimit; ++Index)
		{
			_wfopen_s(&FilePointer, Directory, FileMode);

			if (nullptr != FilePointer)
			{
				break;
			}
		}

		if (nullptr == FilePointer)
		{
			CCrashDump::Crash();
		}
	}

	InterlockedIncrement(&_LogCount);

	CString Text;

	WriteLogTime(Text, LogLevel);

	va_list va;

	va_start(va, Format);	

	int DataLength = _vsctprintf(Format, va) + 1;

	TCHAR* Data = new TCHAR[DataLength];

	if (S_OK != StringCbVPrintf(Data, sizeof(TCHAR) * DataLength, Format, va))
	{
		ReleaseSRWLockExclusive(&_SRWLock);
		fclose(FilePointer);
		wprintf(L"Log Text Copy Error\n");
		return;
	}

	va_end(va);

	Text += L" [";
	Text += Data;
	Text += L"]";
	Text += L"\r\n";

	fwprintf_s(FilePointer, L"%s", Text.GetBuffer());	

	fclose(FilePointer);

	wprintf_s(L"%s\n", Text.GetBuffer());

	delete[] Data;
	Data = NULL;

	ReleaseSRWLockExclusive(&_SRWLock);
}

void CLog::MakeDirectory(CString& Directory)
{
	// 경로 찾기
	WIN32_FIND_DATA FileInfo;

	CString		LocalDirectory = L"";

	for (;;)
	{
		CString LocalLog = LocalDirectory + L"Log";		

		if (INVALID_HANDLE_VALUE != FindFirstFile(LocalLog, &FileInfo))
		{
			break;
		}

		CString LocalWindows = LocalDirectory + L"Windows";

		if (INVALID_HANDLE_VALUE != FindFirstFile(LocalWindows, &FileInfo))
		{
			CreateDirectory(LocalLog.GetBuffer(), NULL);
			break;
		}

		LocalDirectory += L"../";
	}

	Directory = LocalDirectory + L"Log";
}

void CLog::MakeDirectoryLogLevel(CString & Directory, LOG_LEVEL LogLevel)
{
	Directory += L"/";

	Directory += Log_LOG_LEVEL[LogLevel];

	CreateDirectory(Directory.GetBuffer(), NULL);
}

void CLog::MakeDirectoryLogTime(CString & Directory)
{
	CString LocalText;

	LocalText.Format(L"%d", _StructTime.tm_year);
	Directory += L"/" + LocalText + L" Year";
	CreateDirectory(Directory.GetBuffer(), NULL);

	LocalText.Format(L"%02d", _StructTime.tm_mon);	
	Directory += L"/" + LocalText + L" Month";
	CreateDirectory(Directory.GetBuffer(), NULL);

	LocalText.Format(L"%d", _StructTime.tm_mday);	
	Directory += L"/" + LocalText + L" Day";
	CreateDirectory(Directory.GetBuffer(), NULL);
}

void CLog::WriteLogTime(CString& Text, LOG_LEVEL LogLevel)
{
	CString LocalText;

	Text += L"[Log Number : ";	
	LocalText.Format(L"%d", _LogCount);
	Text += LocalText;
	Text += L"] ";

	Text += L"[";
	LocalText.Format(L"%04d", _StructTime.tm_year);
	Text += LocalText + L"-";

	LocalText.Format(L"%02d", _StructTime.tm_mon);
	Text += LocalText + L"-";

	LocalText.Format(L"%02d", _StructTime.tm_mday);
	Text += LocalText + L"-";

	LocalText.Format(L"%02d", _StructTime.tm_hour);
	Text += LocalText + L"-";

	LocalText.Format(L"%02d", _StructTime.tm_min);
	Text += LocalText + L"-";

	LocalText.Format(L"%02d", _StructTime.tm_sec);
	Text += LocalText + L"]";
}

void CLog::Release()
{
}