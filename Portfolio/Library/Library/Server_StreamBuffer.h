#ifndef SERVER_STREAMBUFFER_H__
#define SERVER_STREAMBUFFER_H__

#include "Server_MemoryPoolTLS.h"
#include "Server_Log.h"

class CStreamBuffer
{
private:
#define HEADER_MAX	5
#define HEADER_DEFAULT 150
public:
	CStreamBuffer();
	CStreamBuffer(int MaxSize);
	~CStreamBuffer();
private:
	friend class CSession;
public:
	BYTE*	Get_ReadPointer();
	BYTE*	Get_WritePointer();
	INT		Get_BufferSize();
	INT		Get_UseSize();
	INT		Get_FreeSize();
	int		Get_Data(BYTE* OutPointer, int Size);
public:
	DWORD	Get_ReferenceCount();	
	DWORD	Get_MemoryPoolChunkCount();
public:
	CStreamBuffer*	Alloc();	
	void			Free(CStreamBuffer* Buffer);
public:
	BYTE	MakeCheckSum();
public:
	int		InsertData(BYTE* InPointer, int Size);
public:
	int		MoveReadPosition(int MoveValue);
	int		BackReadPosition(int MoveValue);
	int		MoveWritePosition(int MoveValue);	
	int		BackWritePosition(int MoveValue);
public:
	CStreamBuffer& operator = (CStreamBuffer& Message);
public:	
	CStreamBuffer& operator << (CHAR Data);
	CStreamBuffer& operator << (BYTE Data);
	CStreamBuffer& operator << (TCHAR Data);
	CStreamBuffer& operator << (SHORT Data);
	CStreamBuffer& operator << (WORD Data);
	CStreamBuffer& operator << (INT Data);
	CStreamBuffer& operator << (UINT Data);
	CStreamBuffer& operator << (LONG Data);
	CStreamBuffer& operator << (DWORD Data);
	CStreamBuffer& operator << (INT64 Data);
	CStreamBuffer& operator << (UINT64 Data);
	CStreamBuffer& operator << (FLOAT Data);
	CStreamBuffer& operator << (double Data);
	CStreamBuffer& operator << (long double Data);
public:
	CStreamBuffer& operator >> (CHAR& Data);
	CStreamBuffer& operator >> (BYTE& Data);
	CStreamBuffer& operator >> (TCHAR& Data);
	CStreamBuffer& operator >> (SHORT& Data);
	CStreamBuffer& operator >> (WORD& Data);
	CStreamBuffer& operator >> (INT& Data);
	CStreamBuffer& operator >> (UINT& Data);
	CStreamBuffer& operator >> (LONG& Data);
	CStreamBuffer& operator >> (DWORD& Data);
	CStreamBuffer& operator >> (INT64& Data);
	CStreamBuffer& operator >> (UINT64& Data);
	CStreamBuffer& operator >> (FLOAT& Data);
	CStreamBuffer& operator >> (double& Data);
	CStreamBuffer& operator >> (long double& Data);
public:
	void	BufferClear();
private:
	void	HeaderSetUP(BYTE* HeaderData, int HeaderSize = HEADER_MAX, int InsertIndex = 3);
private:
	static CMemoryPoolTLS<CStreamBuffer>		_MemoryPool;
private:
	BYTE*										_PreBuffer;
private:	
	INT											_Size;
	INT											_ReadPosition;
	INT											_WritePosition;
	DWORD										_ReferenceCount;
};

#endif // !SERVER_STREAMBUFFER_H__