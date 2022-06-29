#ifndef __MEMORYPOOLTLS__H__
#define __MEMORYPOOLTLS__H__

#include "LockFreeMemoryPool.h"

template <typename T>
class CMemoryPoolTLS
{
private:
#define MAX_SIZE	100
private:
	class CChunk
	{
	public:
		typedef struct tagDATA
		{
			CChunk* _Chunk;
			T		_Data;
		}DATA;

	public:
		CChunk()
			: _AllocCount(0), _FreeCount(0)
		{
		}

		~CChunk() {	}
	public:
		DATA							_Data[MAX_SIZE];
		DWORD							_AllocCount;
		DWORD							_FreeCount;
	};
public:
	CMemoryPoolTLS();
	~CMemoryPoolTLS();
public:
	DWORD								Get_AllocChunkCount();	
public:
	T*									Alloc(bool Placement = false);
	bool								Free(T* Data);
	CChunk*								CreateChunk(bool Placement = false);
public:
	CLockFreeMmemoryPool<CChunk>*		_MemoryPool;
private:
	DWORD								_TLSAlloc;
	DWORD								_ChunkCount;			// 만든 Chunk의 개수	
};

template<typename T>
inline CMemoryPoolTLS<T>::CMemoryPoolTLS()
	: _ChunkCount(0)
{
	_TLSAlloc = TlsAlloc();

	if (TLS_OUT_OF_INDEXES == _TLSAlloc)
	{
		CCrashDump::Crash();
	}

	_MemoryPool = new CLockFreeMmemoryPool<CChunk>();
}

template<typename T>
inline CMemoryPoolTLS<T>::~CMemoryPoolTLS()
{
	TlsFree(_TLSAlloc);

	delete _MemoryPool;
	_MemoryPool = nullptr;
}

template<typename T>
inline DWORD CMemoryPoolTLS<T>::Get_AllocChunkCount()
{
	return _ChunkCount;
}

template<typename T>
inline T* CMemoryPoolTLS<T>::Alloc(bool Placement)
{
	CChunk*	TempChunk = (CChunk*)TlsGetValue(_TLSAlloc);

	if (nullptr == TempChunk)
	{
		TempChunk = CreateChunk();
	}

	DWORD AllocCount = InterlockedIncrement(&TempChunk->_AllocCount);

	if (MAX_SIZE <= AllocCount)
	{  
		CreateChunk();
	}

	if (true == Placement)
	{
		new (&TempChunk->_Data[AllocCount - 1]) T;
	}

	return &TempChunk->_Data[AllocCount - 1]._Data;
}

template<typename T>
inline bool CMemoryPoolTLS<T>::Free(T* Data)
{
	CChunk** TempChunk = (CChunk**)(((BYTE*)Data) - sizeof(CChunk*));

	DWORD FreeCount = InterlockedIncrement(&((*TempChunk)->_FreeCount));

	if (MAX_SIZE <= FreeCount)
	{
		InterlockedDecrement(&_ChunkCount);
		_MemoryPool->Free(*TempChunk);
	}

	return true;
}

template<typename T>
inline typename CMemoryPoolTLS<T>::CChunk* CMemoryPoolTLS<T>::CreateChunk(bool Placement)
{
	CChunk* TempChunk = _MemoryPool->Alloc(Placement);

	if (false == TlsSetValue(_TLSAlloc, TempChunk))
	{
		CCrashDump::Crash();
	}

	if (TempChunk->_FreeCount >= MAX_SIZE)
	{
		TempChunk->_AllocCount = 0;
		TempChunk->_FreeCount = 0;
	}

	for (int Index = 0; Index < MAX_SIZE; ++Index)
	{
		TempChunk->_Data[Index]._Chunk = TempChunk;
	}

	InterlockedIncrement(&_ChunkCount);

	return TempChunk;
}

#endif