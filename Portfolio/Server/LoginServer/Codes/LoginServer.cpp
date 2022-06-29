#include "pch.h"
#include "LoginServer.h"

CLoginServer::CLoginServer()
	:	_MonitorFPS(0),	
		_MonitorPort(0)
{
	memset(_MonitorIP, 0, sizeof(TCHAR) * IP_MAX);
}

CLoginServer::~CLoginServer()
{
	Release();
}

unsigned int __stdcall CLoginServer::MonitorThread(LPVOID Parameter)
{
	CLoginServer*	LoginServer = (CLoginServer*)Parameter;

	DWORD			Transferred = 0;

	CMonitorSession*		Session = nullptr;

	NEWOVERLAPPED*	NewOverlapped = nullptr;

	for (;;)
	{
		bool GetQueueResult = GetQueuedCompletionStatus(LoginServer->_MonitorIOCP, &Transferred, (PULONG_PTR)&Session, (LPOVERLAPPED*)&NewOverlapped, INFINITE);

		/*
		IOCP 에러
		*/
		if (nullptr == NewOverlapped)
		{
			CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"MonitorThread IOCP.txt", L"%d", 10);
			break;
		}

		/*
		연결 끊긴 상황
		*/
		if (0 == Transferred)
		{
			CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"MonitorThread Connect.txt", L"%d", 10);
		}

		/*
		Recv
		*/
		if (OVERLAPPED_RECV == NewOverlapped->_State)
		{
		}

		/*
		Send
		*/
		if (OVERLAPPED_SEND == NewOverlapped->_State)
		{

		}

		InterlockedIncrement64(&LoginServer->_MonitorFPS);
	}

	return 0;
}

void CLoginServer::ConnectMonitor()
{
	WSADATA	wsa;

	WSAStartup(MAKEWORD(2, 2), &wsa);

	_MonitorSocket = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN ServerAddr;

	ServerAddr.sin_family = AF_INET;
	InetPton(AF_INET, _MonitorIP, &ServerAddr.sin_addr.s_addr);
	WSAHtons(_MonitorSocket, _MonitorPort, &ServerAddr.sin_port);
}

bool CLoginServer::ServerClose()
{
	bool Result = false;

	return Result;
}

void CLoginServer::Release()
{
}