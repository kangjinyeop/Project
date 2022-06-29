#ifndef ENGINE_LOG_H__
#define ENGINE_LOG_H__

#include <atlstr.h>
#include <strsafe.h>
#include <ctime>

#include "Engine_Macro.h"
#include "Engine_CrashDump.h"

BEGIN(Engine)

class ENGINE_DLL CLog
{
	DECLARE_SINGLETON(CLog)
private:
#define MAX_CREATECOUNT 10
private:
#define BEGIN_ENUMTEXT( NAME ) const char* Log_##NAME[] = {
#define ENUM_ENUMTEXT( ELEM ) #ELEM
#define END_ENUMTEXT( NAME ) };
public:
	typedef enum LOG_LEVEL
	{
		LOG_LEVEL_DEBUG,
		LOG_LEVEL_WARNING,
		LOG_LEVEL_ERROR,
		LOG_LEVEL_SYSTEM,
		LOG_LEVEL_END,
	}LOG_LEVEL;

	typedef enum LOG_MODE
	{
		LOG_MODE_TEXT,
		LOG_MODE_BINARY,
		LOG_MODE_END,
	}LOG_MODE;
private:
	explicit CLog();
public:
	~CLog();
public:
	// 로그 파일 만들기	
	void CreateLogFile(LOG_LEVEL LogLevel, LOG_MODE LogMode, const TCHAR* FileName, const TCHAR* Format, ...);
public:
	LOG_LEVEL			_LogLevel;
private:
	void				MakeDirectory(CString& Directory);
	void				MakeDirectoryLogLevel(CString& Directory, LOG_LEVEL LogLevel);
	void				MakeDirectoryLogTime(CString& Directory);
	void				WriteLogTime(CString& Text, LOG_LEVEL LogLevel);
	void				Release();
private:
	SRWLOCK				_SRWLock;
	DWORD				_LogCount;
	tm					_StructTime;
	int					_FileCreateLimit;
};

END

#endif // !ENGINE_LOG_H__