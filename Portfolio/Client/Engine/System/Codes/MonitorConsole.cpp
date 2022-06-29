#include "stdafx.h"
#include "MonitorConsole.h"

USING(Engine)

IMPLEMENT_SINGLETON(CMonitorConsle)

CMonitorConsle::CMonitorConsle()
	: ConsoleSwitch(false)
{
}

CMonitorConsle::~CMonitorConsle()
{
	Release();
}

void CMonitorConsle::CreateConsole()
{
	if (true == ConsoleSwitch)
	{
		AllocConsole();
	}	
}

void CMonitorConsle::ConsolePrint(const TCHAR * ConsoleText)
{
	if (true == ConsoleSwitch)
	{
		DWORD Byte = 0;

		HANDLE HandleConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		WriteFile(HandleConsole, ConsoleText, lstrlen(ConsoleText), &Byte, NULL);
	}	
}

void CMonitorConsle::Release()
{
	if (true == ConsoleSwitch)
	{
		FreeConsole();
	}
}