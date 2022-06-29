#ifndef SESSION_H__
#define SESSION_H__

#include "Server_RingBuffer.h"
#include "Server_StreamBuffer.h"
#include "Server_LockFreeQueue.h"
#include "Server_Queue.h"

class CSession
{
public:
	enum OverlappedState
	{
		OVERLAPPED_SEND = 0,
		OVERLAPPED_RECV = OVERLAPPED_SEND + 1,
		OVERLAPPED_END	= OVERLAPPED_SEND + 2,
	};

	typedef struct tagOverlapped
	{
		WSAOVERLAPPED		_Overlapped;
		OverlappedState		_State;
	}NEWOVERLAPPED;
private:
	typedef struct tagClientInfo
	{
	private:
#define MAX_IP 16
	public:
		SOCKET	_Socket;
		WORD	_Port;
		TCHAR	_IP[MAX_IP];
	}CLIENTINFO;
private:
	friend class CLanServer;
public:
	explicit CSession();
	virtual ~CSession();
public:
	virtual void PacketProcess() PURE;
private:
	void Release();
protected:
	CLIENTINFO						_ClientInfo;
	NEWOVERLAPPED					_SendOverlapped;
	NEWOVERLAPPED					_RecvOverlapped;
	CRingBuffer						_RecvRingBuffer;	
	CLockFreeQueue<CStreamBuffer*>	_SendRingBuffer;
	CCustomQueue<CStreamBuffer*>	_CompleteMessage;
	INT64							_SessionID;
	INT								_HeartBeat;
	bool							_UseFlag;
};

#endif // !SESSION_H__