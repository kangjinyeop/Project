#ifndef SERVER_RINGBUFFER_H__
#define SERVER_RINGBUFFER_H__

#define BLANK 1

#include "Server_CrashDump.h"

class CRingBuffer
{
private:
#define MAX_WSASENDBUFFER 100
private:
	enum BufferSize
	{		
		MIN_SIZE		= 100,
		DEFAULT_SIZE	= 9999,
	};
	enum BufferMessage
	{
		SIZEERROR		= -1,
		DATAERROR		= -1
	};
public:
	CRingBuffer();
	CRingBuffer(int MaxSize);
	~CRingBuffer();
public:
	int		Get_BufferSize()						const;		// �޸� ���� �뷮
	int		Get_UseSize()							const;		// ��� ���� �뷮
	int		Get_FreeSize()							const;		// ��밡���� �뷮
	int		Get_DirectEnqueueSize()					const;		// �������� ���� �뷮
	int		Get_DirectDequeueSize(int Buffer = 0)	const;		// �������� ���� �뷮	
	int		Get_FrontPosition()						const;
	int		Get_RearPosition()						const;
	BYTE*	Get_PreBuffer()							const;
	BYTE*	Get_FrontPointer()						const;
	BYTE*	Get_RearPointer()						const;
public:
	void	Initialize(int MaxSize);
public:
	int		Enqueue(BYTE* InData, int DataSize);
	int		Dequeue(BYTE* OutData, int DataSize);
	int		Peek(BYTE* OutData, int DataSize);
	int		MoveFront(int FrontPosition);
	int		BackFront(int FrontPosition);
	int		MoveRear(int RearPosition);
	int		BackRear(int RearPosition);
public:
	void	ClearBuffer();
private:	// �޸�
	BYTE*	_PreBuffer;
private:	// ��ġ��
	int		_FrontPosition;
	int		_RearPosition;
	int		_Size;
};

#endif // !SERVER_RINGBUFFER_H__