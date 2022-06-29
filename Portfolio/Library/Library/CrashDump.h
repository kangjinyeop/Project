#ifndef Engine_CrashDump_H__
#define Engine_CrashDump_H__

class CCrashDump
{
public:
	CCrashDump();
	~CCrashDump();
public:
	static void Crash();
	static LONG WINAPI MyExceptionFilter(__in PEXCEPTION_POINTERS pExceptionPointer);
	static void SetHandlerDump();
	static void myInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);
	static int _custom_Report_hook(int ireposttype, char * message, int * returnvalue);
	static void myPurecallHandler();
public:
	static long _DumpCount;
};

#endif // !Engine_CrashDump_H__