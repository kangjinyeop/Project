#include "stdafx.h"
#include "Server_RingBuffer.h"

CRingBuffer::CRingBuffer()
	:	_FrontPosition(0),	_RearPosition(0)
{
	_Size = DEFAULT_SIZE + 1;

	_PreBuffer = new BYTE[_Size];

	if (nullptr == _PreBuffer)
	{
		CCrashDump::Crash();
	}
}

CRingBuffer::CRingBuffer(int MaxSize)
	:	_Size(MaxSize + 1), _FrontPosition(0), _RearPosition(0)
{
	_PreBuffer = new BYTE[_Size];

	if (nullptr == _PreBuffer)
	{
		CCrashDump::Crash();
	}
}

CRingBuffer::~CRingBuffer()
{
	delete[] _PreBuffer;
	_PreBuffer = nullptr;
}

int CRingBuffer::Get_BufferSize() const
{
	return _Size;
}

int CRingBuffer::Get_UseSize() const
{
	int TempFront = _FrontPosition;
	int TempRear = _RearPosition;
	int UseSize = 0;

	if (TempFront <= TempRear)
	{
		UseSize = TempRear - TempFront;
	}

	else
	{
		UseSize =  (_Size - TempFront) + TempRear;
	}

	return UseSize;
}

int CRingBuffer::Get_FreeSize() const
{
	int TempFront = _FrontPosition;
	int TempRear = _RearPosition;
	int FresSize = 0;

	if (TempRear < TempFront)
	{
		FresSize = TempFront - TempRear - BLANK;
	}

	else
	{
		FresSize = (_Size - TempRear) + TempFront - BLANK;
	}

	return FresSize;
}

int CRingBuffer::Get_DirectEnqueueSize() const
{
	int TempFront = _FrontPosition;
	int TempRear = _RearPosition;
	int Size = 0;

	/*
	Front가 Rear 보다 앞인경우
	*/
	if (TempRear < TempFront)
	{
		Size = TempFront - TempRear - BLANK;
	}

	else
	{
		Size = _Size - TempRear;

		if (0 == TempFront)
		{
			Size -= BLANK;
		}
	}

	return Size;
}

int CRingBuffer::Get_DirectDequeueSize(int Buffer) const
{
	int TempFront = _FrontPosition;

	if (0 != Buffer)
	{
		TempFront = Buffer;
	}

	int TempRear = _RearPosition;
	int Size = 0;

	/*
	Front가 Rear 보다 뒤인경우
	*/
	if (TempFront <= TempRear)
	{
		Size = TempRear - TempFront;
	}

	/*
	Front가 Rear 보다 앞인경우
	*/
	else
	{
		Size = _Size - TempFront;
	}

	/*
	Front 와 Rear의 위치가 같은 경우
	*/
	return Size;
}

int CRingBuffer::Get_FrontPosition() const
{
	return _FrontPosition;
}

int CRingBuffer::Get_RearPosition() const
{
	return _RearPosition;
}

BYTE * CRingBuffer::Get_PreBuffer() const
{
	return &_PreBuffer[0];
}

BYTE * CRingBuffer::Get_FrontPointer() const
{
	return &_PreBuffer[_FrontPosition];
}

BYTE * CRingBuffer::Get_RearPointer() const
{
	return &_PreBuffer[_RearPosition];
}

void CRingBuffer::Initialize(int MaxSize)
{
	_Size = MaxSize + 1;

	_PreBuffer = new BYTE[_Size];

	if (nullptr == _PreBuffer)
	{
		CCrashDump::Crash();
	}

	memset(_PreBuffer, 0, sizeof(BYTE) * _Size);

	_FrontPosition = 0;
	_RearPosition = 0;
}

int CRingBuffer::Enqueue(BYTE * InData, int DataSize)
{	
	int DirectSize = Get_DirectEnqueueSize();
	int RearPosition = _RearPosition;

	if (DataSize <= 0)
	{
		return SIZEERROR;
	}	

	/*
	경계 값 보다 작거나 같다.
	*/
	if (DataSize <= DirectSize)
	{
		memcpy(&_PreBuffer[RearPosition], InData, DataSize);
	}

	else
	{
		memcpy(&_PreBuffer[RearPosition], InData, DirectSize);
		memcpy(_PreBuffer, (InData + DirectSize), (DataSize - DirectSize));
	}

	_RearPosition = (RearPosition + DataSize) % _Size;

	return DataSize;
}

int CRingBuffer::Dequeue(BYTE * OutData, int DataSize)
{	
	int UseSize = Get_UseSize();
	int DirectSize = Get_DirectDequeueSize();
	int FrontPosition = _FrontPosition;

	if (DataSize <= 0 || UseSize < DataSize)
	{
		return SIZEERROR;
	}

	/*
	경계 값 보다 작거나 같다.
	*/
	if (DataSize <= DirectSize)
	{
		memcpy_s(OutData, DataSize, &_PreBuffer[FrontPosition], DataSize);
	}

	else
	{
		memcpy_s(OutData, DirectSize, &_PreBuffer[FrontPosition], DirectSize);
		memcpy_s(OutData + DirectSize, DataSize - DirectSize, &_PreBuffer[0], DataSize - DirectSize);
	}

	_FrontPosition = (FrontPosition + DataSize) % _Size;

	return DataSize;
}

int CRingBuffer::Peek(BYTE * OutData, int DataSize)
{
	int UseSize = Get_UseSize();
	int DirectSize = Get_DirectDequeueSize();
	int FrontPosition = _FrontPosition;

	if (DataSize < 0 || UseSize < DataSize)
	{
		return SIZEERROR;
	}

	/*
	경계 값 보다 작거나 같다.
	*/
	if (DataSize <= DirectSize)
	{
		memcpy_s(OutData, DataSize, &_PreBuffer[FrontPosition], DataSize);
	}

	else
	{
		memcpy_s(OutData, DirectSize, &_PreBuffer[FrontPosition], DirectSize);
		memcpy_s(OutData + DirectSize, DataSize - DirectSize, &_PreBuffer[0], DataSize - DirectSize);
	}

	return DataSize;
}

int CRingBuffer::MoveFront(int FrontPosition)
{
	int UseSize = Get_UseSize();
	int TempFront = _FrontPosition;

	if (UseSize < FrontPosition)
	{
		return 0;
	}

	_FrontPosition = (TempFront + FrontPosition) % _Size;

	return FrontPosition;
}

int CRingBuffer::BackFront(int FrontPosition)
{
	int UseSize = Get_UseSize();
	int TempFront = _FrontPosition;

	if (0 > FrontPosition)
	{
		return 0;
	}

	_FrontPosition = (TempFront - FrontPosition + _Size) % _Size;

	return FrontPosition;
}

int CRingBuffer::MoveRear(int RearPosition)
{
	int FresSize = Get_FreeSize();
	int TempRear = _RearPosition;

	if (FresSize < RearPosition)
	{
		return 0;
	}

	_RearPosition = (TempRear + RearPosition) % _Size;

	return RearPosition;
}

int CRingBuffer::BackRear(int RearPosition)
{
	int FresSize = Get_FreeSize();
	int TempRear = _RearPosition;

	if (0 > RearPosition)
	{
		return 0;
	}

	_RearPosition = (TempRear - RearPosition + _Size) % _Size;

	return RearPosition;
}

void CRingBuffer::ClearBuffer()
{
	_FrontPosition = 0;
	_RearPosition = 0;
}