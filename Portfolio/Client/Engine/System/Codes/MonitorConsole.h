#ifndef MONITOR_CONSOLE_H__
#define MONITOR_CONSOLE_H__

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CMonitorConsle
{
	DECLARE_SINGLETON(CMonitorConsle)
private:
	explicit CMonitorConsle(void);
	~CMonitorConsle();
public:
	void CreateConsole();
	void ConsolePrint(const TCHAR* ConsoleText);
public:
	bool ConsoleSwitch;
private:
	void Release();
};

END

#endif // !MONITOR_CONSOLE_H__