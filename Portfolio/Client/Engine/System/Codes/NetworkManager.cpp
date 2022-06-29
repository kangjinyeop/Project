#include "stdafx.h"
#include "NetworkManager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CNetworkManager)

CNetworkManager::CNetworkManager()
	:	_ConnectHandle(nullptr),	_WorkerHandle(nullptr), _ConnectEvent(nullptr),
		_IOCPWorker(nullptr), _WorkerThreadCount(0), _ActiveWorkerThreadCount(0)
{
}

CNetworkManager::~CNetworkManager()
{
	Release();
}

unsigned int __stdcall CNetworkManager::ConnectThread(LPVOID Parameter)
{
	CNetworkManager*		Server = (CNetworkManager*)Parameter;	

	for (;;)
	{
		WaitForSingleObject(Server->_ConnectEvent, INFINITE);

		Server->LoadSetUP();

		Server->ServerInitialize();

		ResetEvent(Server->_ConnectEvent);
	}

	return 0;
}

unsigned int __stdcall CNetworkManager::WorkerThread(LPVOID Parameter)
{
	CNetworkManager*		Server = (CNetworkManager*)Parameter;

	DWORD					Transferred = 0;

	SESSION*				Session = nullptr;

	NEWOVERLAPPED*			Overlapped = nullptr;

	for (;;)
	{
		GetQueuedCompletionStatus(Server->_IOCPWorker, &Transferred, (PULONG_PTR)&Session, (LPOVERLAPPED*)&Overlapped, INFINITE);

		if (nullptr == Overlapped)
		{
			CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_BINARY, L"WorkThread Overlapped.txt", L"%lld", Session->_Socket);
			CCrashDump::Crash();
		}

		if (0 == Transferred)
		{
			CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_BINARY, L"WorkThread Transferred.txt", L"%lld", Session->_Socket);
			CCrashDump::Crash();
		}

		if (OVERLAPPED_RECV == Overlapped->_State)
		{
			for (;;)
			{

			}
		}

		else if (OVERLAPPED_SEND == Overlapped->_State)
		{

		}
	}

	return 0;
}

void CNetworkManager::LoadSetUP()
{
	CTextParser TextParser;

	TextParser.LoadFile(L"../Data/EngineNetwork.txt");

	TextParser.ObjectData(L"Network", L"Port", &_Port, CTextParser::TYPE_short);

	TextParser.ObjectString(L"Network", L"IP", &_IP);

	TextParser.ObjectData(L"Network", L"WorkerThread", &_WorkerThreadCount, CTextParser::TYPE_int);

	TextParser.ObjectData(L"Network", L"ActiveWorkerThread", &_ActiveWorkerThreadCount, CTextParser::TYPE_int);
}

void CNetworkManager::ServerInitialize()
{
	_ConnectEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	_ConnectHandle = (HANDLE)_beginthreadex(NULL, 0, ConnectThread, this, NULL, NULL);

	_WorkerHandle = new HANDLE[_WorkerThreadCount];

	for (DWORD Index = 0; Index < _WorkerThreadCount; ++Index)
	{
		_WorkerHandle[Index] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, NULL, NULL);
	}	
}

void CNetworkManager::ServerClose()
{
	SetEvent(_ConnectEvent);

	closesocket(_Session._Socket);
	_Session._Socket = INVALID_SOCKET;
}

void CNetworkManager::Release()
{
	CloseHandle(_ConnectHandle);

	for (DWORD WorkerIndex = 0; WorkerIndex < WorkerIndex; ++WorkerIndex)
	{
		CloseHandle(_WorkerHandle[WorkerIndex]);
	}

	if (nullptr == _WorkerHandle)
	{
		delete[] _WorkerHandle;
		_WorkerHandle = nullptr;
	}	

	if (nullptr != _IP)
	{
		delete[] _IP;
		_IP = nullptr;
	}
}