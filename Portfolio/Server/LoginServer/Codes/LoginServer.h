#ifndef LOGINSERVER_H__
#define LOGINSERVER_H__

#include "MonitorSession.h"

class CLoginServer
{
private:
#define IP_MAX 16
public:
	explicit CLoginServer();
	~CLoginServer();
public:
	static unsigned int WINAPI	MonitorThread(LPVOID Parameter);
public:
	static unsigned int WINAPI	AcceptThread(LPVOID Parameter);
	static unsigned int WINAPI	WorkerThread(LPVOID Parameter);
public:
	void ConnectMonitor();
public:
	bool ServerClose();
public:
	INT64								_MonitorFPS;
	INT64								_AcceptTotal;
	INT64								_AcceptFPS;	
	INT64								_WorkerFPS;
	INT64								_RecvFPS;
	INT64								_RecvTPS;
	INT64								_SendFPS;
	INT64								_SendTPS;
private:
	void Release();
// Monitor
private:
	TCHAR		_MonitorIP[IP_MAX];
	DWORD		_MonitorPort;
	SOCKET		_MonitorSocket;
	HANDLE		_MonitorIOCP;
};

#endif // !LOGINSERVER_H__