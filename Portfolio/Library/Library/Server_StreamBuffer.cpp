#include "stdafx.h"
#include "Server_StreamBuffer.h"

CMemoryPoolTLS<CStreamBuffer> CStreamBuffer::_MemoryPool;

CStreamBuffer::CStreamBuffer()
	:	_Size(HEADER_DEFAULT + HEADER_MAX), _ReadPosition(HEADER_MAX), _WritePosition(HEADER_MAX),
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
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer Get Data Error.txt", L"Size : %d", _Size);
		CCrashDump::Crash();
	}

	memcpy(OutPointer, &_PreBuffer[_ReadPosition], Size);
	_ReadPosition += Size;

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

int CStreamBuffer::InsertData(BYTE * InPointer, int Size)
{
	if (Size <= -1 || nullptr == InPointer)
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer Get Data Error.txt", L"Size : %d", _Size);
		CCrashDump::Crash();
	}

	if ((_WritePosition + Size) > _Size)
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer Resize.txt", L"Before Size : %d || After Size : %d || Size : %d", _Size, _Size * 2, Size);

		BYTE* NewBuffer = new BYTE[(_Size - HEADER_MAX) * 2 + HEADER_MAX];

		memcpy(NewBuffer, _PreBuffer, sizeof(BYTE) * _Size);

		delete[] _PreBuffer;
		_PreBuffer = nullptr;

		_PreBuffer = NewBuffer;

		_Size = (_Size - HEADER_MAX) * 2 + HEADER_MAX;
	}

	memcpy(&_PreBuffer[_WritePosition], InPointer, Size);
	_WritePosition += Size;

	return 0;
}

int CStreamBuffer::MoveReadPosition(int MoveValue)
{
	INT NewPosition = _ReadPosition + MoveValue;

	if (NewPosition <= -1 || NewPosition > _WritePosition)
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer MoveReadPosition Error.txt", L"Size : %d || ReadPosition : %d || MoveValue : %d", _Size, _ReadPosition, MoveValue);
		CCrashDump::Crash();
	}

	_ReadPosition += MoveValue;

	return 0;
}

int CStreamBuffer::BackReadPosition(int MoveValue)
{
	INT NewPosition = _ReadPosition - MoveValue;

	if (NewPosition <= -1)
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer BackReadPosition Error.txt", L"Size : %d || ReadPosition : %d || MoveValue : %d", _Size, _ReadPosition, MoveValue);
		CCrashDump::Crash();
	}

	_ReadPosition -= MoveValue;

	return 0;
}

int CStreamBuffer::MoveWritePosition(int MoveValue)
{
	INT NewPosition = _WritePosition + MoveValue;

	if (NewPosition <= -1 || NewPosition > _Size)
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer MoveWritePosition Error.txt", L"Size : %d || WritePosition : %d || MoveValue : %d", _Size, _WritePosition, MoveValue);
		CCrashDump::Crash();
	}

	_WritePosition += MoveValue;

	return 0;
}

int CStreamBuffer::BackWritePosition(int MoveValue)
{
	INT NewPosition = _WritePosition - MoveValue;

	if (NewPosition <= -1)
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"StreaBuffer BackWritePosition Error.txt", L"Size : %d || WritePosition : %d || MoveValue : %d", _Size, _WritePosition, MoveValue);
		CCrashDump::Crash();
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