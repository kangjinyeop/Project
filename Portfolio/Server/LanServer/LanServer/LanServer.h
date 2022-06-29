#ifndef LAN_SERVER_H__
#define LAN_SERVER_H__

#include "Session.h"
#include "Server_TextParser.h"

class CLanServer
{
private:
#define SLEEP_TIME 5
private:
	enum SERVER_STATE
	{
		SERVER_STOP = 0,
		SERVER_START = 1,
		SERVER_EIXT = 2,
	};
	typedef struct tagKeepAlive
	{
		DWORD	_OnOff;
		DWORD	_Time;
		DWORD	_Interval;
	}KEEPALIVE;
#pragma pack(push, 1) 
	typedef struct tagNetworkHeader
	{
		BYTE				_Code;
		WORD				_Len;
		BYTE				_RandomKey;
		BYTE				_CheckSum;
	}HEADER;
#pragma pack(pop) 
public:
	explicit CLanServer();
	virtual ~CLanServer();
public:
	static unsigned int WINAPI	AcceptThread(LPVOID Parameter);
	static unsigned int WINAPI	WorkerThread(LPVOID Parameter);
	static unsigned int WINAPI	SendThread(LPVOID Parameter);
	static unsigned int WINAPI	MonitorThread(LPVOID Parameter);
public:
	void						ServerStart();
	void						ServerStop();
	void						ServerClose();
private:
	void						MakeCheckSum(CStreamBuffer* Buffer);
	void						Encoding(BYTE* Buffer, HEADER Header);
	void						Decoding();
private:
	void						RecvPost(CSession * Session);
private:
	void						DataParser(const TCHAR* ParserName);
	CSession*					FindSession();
	void						Release();
public:	
	INT64						_AcceptTotal;
	DWORD						_AcceptCount;
	DWORD						_AcceptFPS;
	DWORD						_WorkerFPS;
	DWORD						_RecvTPS;
	DWORD						_SendFPS;
	DWORD						_SendTPS;
private:
	HANDLE						_AcceptHandle;
	HANDLE*						_WorkerHandle;
	HANDLE						_SendHandle;	
	HANDLE						_MonitorHandle;
private:
	SOCKET						_listen_socket;
	HANDLE						_IOCPWorker;
	SERVER_STATE				_ServerState;
private:
	TCHAR*						_IP;
	WORD						_Port;
	DWORD						_WorkerThreadCount;
	DWORD						_ActiveWorkerThreadCount;	
	KEEPALIVE					_Keepalive;
	DWORD						_MaxClient;
	DWORD						_HeartBeat;
	char						_Packet_Code;
	char						_PacketKey;
private:
	CSession**					_Session;
};

#endif // !LAN_SERVER_H__