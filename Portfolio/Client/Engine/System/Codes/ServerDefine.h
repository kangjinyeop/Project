#ifndef SERVER_DEFINE_H__
#define SERVER_DEFINE_H__

namespace Engine
{
	enum OverlappedState
	{
		OVERLAPPED_SEND = 0,
		OVERLAPPED_RECV = 1,
		OVERLAPPED_END = 2,
	};

	typedef struct tagOverlapped
	{
		WSAOVERLAPPED		_Overlapped;
		OverlappedState		_State;
	}NEWOVERLAPPED;

#pragma pack(push, 1) 
	typedef struct tagHeader
	{
		BYTE				_Code;
		WORD				_Len;
		BYTE				_RandomKey;
		BYTE				_CheckSum;
	}HEADER;
#pragma pack(pop) 

	typedef struct tagSession
	{
		SOCKET							_Socket;
		NEWOVERLAPPED					_SendOverlapped;
		NEWOVERLAPPED					_RecvOverlapped;
		CRingBuffer						_RecvRigBuffer;
		CLockFreeQueue<CStreamBuffer*>	_SendRigBuffer;
		bool							_HeartBeatCheck;
	}SESSION;
}

#endif // !SERVER_DEFINE_H__