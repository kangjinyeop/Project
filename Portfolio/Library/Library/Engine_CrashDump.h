#ifndef ENGINE_CRASHDUMP_H__
#define ENGINE_CRASHDUMP_H__

#include <crtdbg.h>
#include <stdio.h>
#include <DbgHelp.h>

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CCrashDump
{
public:
	explicit CCrashDump();
	~CCrashDump();
public:
	static void Crash();
	static LONG WINAPI MyExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer);
	static void SetHandlerDump();
	static void myInvalidParameterHandler(const TCHAR* expression, const TCHAR* function, const TCHAR* file, UINT line, uintptr_t pReserved);
	static int _custom_Report_hook(int ireposttype, char * message, int * returnvalue);
	static void myPurecallHandler();
public:
	static long _DumpCount;
};

END

#endif // !ENGINE_CRASHDUMP_H__