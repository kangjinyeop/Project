#include "pch.h"

#include "LoginServer.h"

int main()
{
	CCrashDump	CrashDump;

	CLoginServer LoginServer;

	LoginServer.ConnectMonitor();	

	timeBeginPeriod(1);

	for (;;)
	{
		if (true == LoginServer.ServerClose())
		{
			break;
		}
	}

	timeEndPeriod(0);

	return 0;
}