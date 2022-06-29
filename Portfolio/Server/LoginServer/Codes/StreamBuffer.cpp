#include "pch.h"
#include "StreamBuffer.h"

CMemoryPoolTLS<CStreamBuffer> CStreamBuffer::_MemoryPool;

CStreamBuffer::CStreamBuffer()
	:	_Size(DEFUALT + HEADER_MAX), _ReadPosition(HEADER_MAX), _WritePosition(HEADER_MAX),
		_ReferenceCount(1)
{
	_PreBuffer = new BYTE[_Size];

	memset(_PreBuffer, 0, sizeof(BYTE) * _Size);
}

CStreamBuffer::CStreamBuffer(int MaxSize)
	:	_Size(MaxSize + HEADER_MAX), _ReadPosition(HEADER_MAX), _WritePosition(HEADER_MAX),
		_ReferenceCount(1)
{
	_PreBuffer = new BYTE[_Size];

	memset(_PreBuffer, 0, sizeof(BYTE) * _Size);
}

CStreamBuffer::~CStreamBuffer()
{
	delete[] _PreBuffer;
	_PreBuffer = nullptr;
}

BYTE * CStreamBuffer::Get_ReadPointer()
{
	return &_PreBuffer[_ReadPosition];
}

BYTE * CStreamBuffer::Get_WritePointer()
{
	return &_PreBuffer[_WritePosition];
}

INT CStreamBuffer::Get_BufferSize()
{
	return _Size - HEADER_MAX;
}

INT CStreamBuffer::Get_UseSize()
{	
	return _WritePosition - _ReadPosition;
}

INT CStreamBuffer::Get_FreeSize()
{
	return _Size - _ReadPosition - HEADER_MAX;
}

int CStreamBuffer::Get_Data(BYTE * OutPointer, int Size)
{
	if (Size <= -1 || nullptr == OutPointer)
	{
		return ERRORVALUE;
	}

	memcpy(OutPointer, &_PreBuffer[_ReadPosition], Size);
	_ReadPosition += Size;

	return 0;
}

int CStreamBuffer::InsertData(BYTE * InPointer, int Size)
{
	if (Size <= -1 || nullptr == InPointer || (_WritePosition + Size) > _Size)
	{
		return ERRORVALUE;
	}

	memcpy(&_PreBuffer[_WritePosition], InPointer, Size);
	_WritePosition += Size;

	return 0;
}

DWORD CStreamBuffer::Get_ReferenceCount()
{
	return _ReferenceCount;
}

DWORD CStreamBuffer::Get_MemoryPoolChunkCount()
{
	return _MemoryPool.Get_AllocChunkCount();
}

CStreamBuffer * CStreamBuffer::Alloc()
{
	CStreamBuffer* Buffer = _MemoryPool.Alloc();

	if (nullptr == Buffer)
	{
		CCrashDump::Crash();
	}

	InterlockedIncrement(&Buffer->_ReferenceCount);

	return Buffer;
}

void CStreamBuffer::Free(CStreamBuffer * Buffer)
{
	InterlockedDecrement(&Buffer->_ReferenceCount);

	Buffer->_WritePosition = HEADER_MAX;
	Buffer->_ReadPosition = HEADER_MAX;

	_MemoryPool.Free(Buffer);
}

BYTE CStreamBuffer::MakeCheckSum()
{
	BYTE CheckSum = 0;
	int Size = _WritePosition;

	for (int Index = HEADER_MAX; Index < Size; ++Index)
	{
		CheckSum += *(_PreBuffer + Index);
	}

	return CheckSum %= 256;
}

bool CStreamBuffer::Encode(BYTE FixKey, BYTE RandKey)
{	
	BYTE PData		= 0;
	BYTE PrePData	= 0;
	BYTE PreEData	= 0;

	BYTE CheckSum = MakeCheckSum();
	int Size = _WritePosition;

	*(_PreBuffer + HEADER_MAX - 1) = CheckSum;

	/*
	P1 = D1 ^ (RK + 1);
	E1 = P1 ^ (K + 1);
	*/
	PData = *(_PreBuffer + HEADER_MAX - 1) ^ (RandKey + 1);
	*(_PreBuffer + HEADER_MAX - 1) = PData ^ (FixKey + 1);

	PrePData = PData;
	PreEData = *(_PreBuffer + HEADER_MAX - 1);

	for (int Index = 0; Index < Size; ++Index)
	{
		/*
		Pn = Dn ^ (P(n - 1) + RK + n);
		En = Pn ^ (E(n - 1) + K + n);
		*/
		PData = *(_PreBuffer + Index + HEADER_MAX) ^ (PrePData + RandKey + Index + 2);
		*(_PreBuffer + Index + HEADER_MAX) = PData ^ (PreEData + FixKey + Index + 2);

		PrePData = PData;
		PreEData = *(_PreBuffer + Index + HEADER_MAX);
	}

	return true;
}

bool CStreamBuffer::Decode(BYTE FixKey, BYTE RandKey, BYTE CheckSum)
{
	BYTE PData			= 0;	// P
	BYTE PrePData		= 0;	// P(n - 1)
	BYTE PreEData		= 0;	// E(n - 1)
	int Size = _WritePosition;

	/*
	P1 = E1 ^ (K + n);
	D1 = P1 ^ (RK + n);
	*/
	PData = CheckSum ^ (FixKey + 1);

	PreEData = CheckSum;

	*(_PreBuffer + HEADER_MAX - 1) = PData ^ (RandKey + 1);

	PrePData = PData;

	for (int Index = 0; Index < _WritePosition; ++Index)
	{
		/*
		Pn = En ^ (E(n - 1) + K + n);
		Dn = Pn ^ (P(n - 1) + RK + n);		
		*/
		PData = *(_PreBuffer + Index + HEADER_MAX) ^ (PreEData + FixKey + Index + 2);

		PreEData = *(_PreBuffer + Index + HEADER_MAX);

		*(_PreBuffer + Index + HEADER_MAX) = PData ^ (PrePData + RandKey + Index + 2);

		PrePData = PData;		
	}

	return true;
}

int CStreamBuffer::MoveReadPosition(int MoveValue)
{
	int TempPosition = _ReadPosition + MoveValue;

	if (TempPosition <= -1 || TempPosition > _WritePosition)
	{
		return ERRORVALUE;
	}

	_ReadPosition += MoveValue;

	return 0;
}

int CStreamBuffer::MoveWritePosition(int MoveValue)
{
	int TempPosition = _WritePosition + MoveValue;

	if (TempPosition <= -1 || TempPosition > _Size)
	{
		return ERRORVALUE;
	}

	_WritePosition += MoveValue;

	return 0;
}

int CStreamBuffer::BackReadPosition(int MoveValue)
{
	int TempPosition = _ReadPosition - MoveValue;

	if (TempPosition <= -1)
	{
		return ERRORVALUE;
	}

	_ReadPosition -= MoveValue;

	return 0;
}

int CStreamBuffer::BackWritePosition(int MoveValue)
{
	int TempPosition = _WritePosition - MoveValue;

	if (TempPosition <= -1)
	{
		return ERRORVALUE;
	}

	_WritePosition -= MoveValue;

	return 0;
}

CStreamBuffer & CStreamBuffer::operator=(CStreamBuffer & Message)
{
	int BufferSize = Message.Get_BufferSize();

	if (nullptr != _PreBuffer)
	{
		delete[] _PreBuffer;
		_PreBuffer = nullptr;
	}

	_PreBuffer = new BYTE[BufferSize];

	memcpy(_PreBuffer, Message._PreBuffer, _Size);

	_Size = BufferSize;
	_ReadPosition = 0;
	_WritePosition = _Size;

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(CHAR Data)
{
	if ((_WritePosition + sizeof(CHAR)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(CHAR));
	
	_WritePosition += sizeof(CHAR);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(BYTE Data)
{
	if ((_WritePosition + sizeof(BYTE)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(BYTE));

	_WritePosition += sizeof(BYTE);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(TCHAR Data)
{
	if ((_WritePosition + sizeof(TCHAR)) > _Size)
	{

	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(TCHAR));

	_WritePosition += sizeof(TCHAR);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(SHORT Data)
{
	if ((_WritePosition + sizeof(SHORT)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(SHORT));

	_WritePosition += sizeof(SHORT);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(WORD Data)
{
	if ((_WritePosition + sizeof(WORD)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(WORD));

	_WritePosition += sizeof(WORD);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(INT Data)
{
	if ((_WritePosition + sizeof(INT)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(INT));

	_WritePosition += sizeof(INT);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(UINT Data)
{
	if ((_WritePosition + sizeof(UINT)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(UINT));

	_WritePosition += sizeof(UINT);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(LONG Data)
{
	if ((_WritePosition + sizeof(LONG)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(LONG));

	_WritePosition += sizeof(LONG);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(DWORD Data)
{
	if ((_WritePosition + sizeof(DWORD)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(DWORD));

	_WritePosition += sizeof(DWORD);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(INT64 Data)
{
	if ((_WritePosition + sizeof(INT64)) > _Size)
	{

	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(INT64));

	_WritePosition += sizeof(INT64);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(UINT64 Data)
{
	if ((_WritePosition + sizeof(UINT64)) > _Size)
	{

	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(UINT64));

	_WritePosition += sizeof(UINT64);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(FLOAT Data)
{
	if ((_WritePosition + sizeof(FLOAT)) > _Size)
	{
		
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(FLOAT));

	_WritePosition += sizeof(FLOAT);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator<<(double Data)
{
	if ((_WritePosition + sizeof(double)) > _Size)
	{
	
	}

	memcpy(&_PreBuffer[_WritePosition], &Data, sizeof(double));

	_WritePosition += sizeof(double);

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(CHAR & Data)
{
	if ((_ReadPosition + sizeof(CHAR)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(CHAR));

		_ReadPosition += sizeof(CHAR);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(BYTE & Data)
{
	if ((_ReadPosition + sizeof(BYTE)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(BYTE));

		_ReadPosition += sizeof(BYTE);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(TCHAR & Data)
{
	if ((_ReadPosition + sizeof(TCHAR)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(TCHAR));

		_ReadPosition += sizeof(TCHAR);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(SHORT & Data)
{
	if ((_ReadPosition + sizeof(SHORT)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(SHORT));

		_ReadPosition += sizeof(SHORT);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(WORD & Data)
{
	if ((_ReadPosition + sizeof(WORD)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(WORD));

		_ReadPosition += sizeof(WORD);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(INT & Data)
{
	if ((_ReadPosition + sizeof(INT)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(INT));

		_ReadPosition += sizeof(INT);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(UINT & Data)
{
	if ((_ReadPosition + sizeof(UINT)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(UINT));

		_ReadPosition += sizeof(UINT);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(LONG & Data)
{
	if ((_ReadPosition + sizeof(LONG)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(LONG));

		_ReadPosition += sizeof(LONG);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(DWORD & Data)
{
	if ((_ReadPosition + sizeof(DWORD)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(DWORD));

		_ReadPosition += sizeof(DWORD);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(INT64 & Data)
{
	if ((_ReadPosition + sizeof(INT64)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(INT64));

		_ReadPosition += sizeof(INT64);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(UINT64 & Data)
{
	if ((_ReadPosition + sizeof(UINT64)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(UINT64));

		_ReadPosition += sizeof(UINT64);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(FLOAT & Data)
{
	if ((_ReadPosition + sizeof(FLOAT)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(FLOAT));

		_ReadPosition += sizeof(FLOAT);
	}

	return *this;
}

CStreamBuffer & CStreamBuffer::operator>>(double & Data)
{
	if ((_ReadPosition + sizeof(double)) <= _WritePosition)
	{
		memcpy(&Data, &_PreBuffer[_ReadPosition], sizeof(double));

		_ReadPosition += sizeof(double);
	}

	return *this;
}

void CStreamBuffer::BufferClear()
{
	_ReadPosition = HEADER_MAX;
	_WritePosition = HEADER_MAX;

	memset(_PreBuffer, 0, sizeof(BYTE) * _Size);
}

void CStreamBuffer::HeaderSetUP(BYTE* HeaderData, int HeaderSize, int InsertIndex)
{
	if (_ReadPosition < HeaderSize)
	{
		return;
	}

	int RerPosition = InsertIndex;

	memcpy(&_PreBuffer[RerPosition], HeaderData, HeaderSize);
	_ReadPosition = RerPosition;
}