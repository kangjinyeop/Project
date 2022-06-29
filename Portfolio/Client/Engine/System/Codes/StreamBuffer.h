#ifndef StreamBuffer_H__
#define StreamBuffer_H__

#include "MemoryPoolTLS.h"

class CSession;
class CStreamBuffer
{
private:
#define HEADER_MAX	5
public:
	enum MessageSize
	{
		MESSAGE_DEFUALT = 150,
		MESSAGE_ERRORVALUE = -1,
		MESSAGE_END,
	};
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
	int		InsertData(BYTE* InPointer, int Size);
public:
	DWORD	Get_ReferenceCount();	
	DWORD	Get_MemoryPoolChunkCount();
public:
	CStreamBuffer*	Alloc();	
	void			Free(CStreamBuffer* Buffer);
public:
	BYTE	MakeCheckSum();
	bool	Encode(BYTE FixKey, BYTE RandKey);			// 5바이트 밀고 넣기
	bool	Decode(BYTE FixKey, BYTE RandKey, BYTE CheckSum);
public:
	int		MoveReadPosition(int MoveValue);
	int		MoveWritePosition(int MoveValue);
	int		BackReadPosition(int MoveValue);
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

#endif // !StreamBuffer_H__