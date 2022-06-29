#ifndef NETWORK_MANAGER_H__
#define NETWORK_MANAGER_H__

BEGIN(Engine)

#include "ServerDefine.h"

class ENGINE_DLL CNetworkManager
{
	DECLARE_SINGLETON(CNetworkManager)
private:
	explicit CNetworkManager();
public:
	~CNetworkManager();
public:
	static unsigned int WINAPI			ConnectThread(LPVOID Parameter);
	static unsigned int WINAPI			WorkerThread(LPVOID Parameter);
public:
	void SendPacket();
private:
	void LoadSetUP();
	void ServerInitialize();
	void ServerClose();
	void Release();
private:
	HANDLE								_ConnectHandle;
	HANDLE*								_WorkerHandle;
	HANDLE								_ConnectEvent;
	HANDLE								_IOCPWorker;
private:
	DWORD								_WorkerThreadCount;
	DWORD								_ActiveWorkerThreadCount;
private:
	WORD								_Port;
	TCHAR*								_IP;
	char								_Packet_Code;
	char								_PacketKey;
private:
	SESSION								_Session;
	DWORD								_HeartBeat;
};

END

#endif // !NETWORK_MANAGER_H__