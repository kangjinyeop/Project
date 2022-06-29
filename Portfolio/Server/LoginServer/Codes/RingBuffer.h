#ifndef RingBuffer_H__
#define RingBuffer_H__

#define BLANK 1

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
	int		Get_BufferSize();							// �޸� ���� �뷮
	int		Get_UseSize();								// ��� ���� �뷮
	int		Get_FreeSize();								// ��밡���� �뷮
	int		Get_DirectEnqueueSize();					// �������� ���� �뷮
	int		Get_DirectDequeueSize(int Buffer = 0);		// �������� ���� �뷮	
	int		Get_FrontPosition();
	int		Get_RearPosition();
	BYTE*	Get_PreBuffer();
	BYTE*	Get_FrontPointer();
	BYTE*	Get_RearPointer();
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

#endif // !RingBuffer_H__