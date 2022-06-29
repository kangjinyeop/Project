#ifndef LockFreeMemoryPool_H__
#define LockFreeMemoryPool_H__

#include "../Include/pch.h"

#define NODE_START	0x65

template <typename T>
class CLockFreeMmemoryPool
{
public:
	typedef struct tagNode
	{			
		tagNode*	_NextNode;		
	}Node;
	typedef struct tagLineNode
	{
		Node*		_Node;
		LONG64		_Index;
	}LINENODE;
public:
	CLockFreeMmemoryPool();
	~CLockFreeMmemoryPool();
public:
	DWORD		Get_TotalCount();
	DWORD		Get_UseCount();
public:
	T*			Alloc(bool PlacementNew = true);
	void		Free(T* Data);
	void		Clear();
	void		Release();
private:
	LINENODE*	_Top;
	DWORD		_TotalCount;		// 총 만든 개수
	DWORD		_UseCount;			// 생성 후 사용하는 개수
	LONG64		_Count;				// 구분 개수
};

template<typename T>
inline CLockFreeMmemoryPool<T>::CLockFreeMmemoryPool()
	: _TotalCount(0), _UseCount(0), _Count(0)
{
	_Top = (LINENODE*)_aligned_malloc(sizeof(LINENODE), 16);
	memset(_Top, 0, sizeof(LINENODE));
}

template<typename T>
inline CLockFreeMmemoryPool<T>::~CLockFreeMmemoryPool()
{
	if (nullptr == _Top->_Node)
	{
		return;
	}

	Node* Temp = (Node*)((char*)_Top->_Node - sizeof(char));
	Node* TempNextNode = nullptr;

	for (;;)
	{
		if (nullptr == Temp)
		{
			break;
		}

		TempNextNode = Temp->_NextNode;

		delete Temp;
		Temp = nullptr;

		Temp = TempNextNode - sizeof(char);
	}

	_aligned_free(_Top);
}

template<typename T>
inline DWORD CLockFreeMmemoryPool<T>::Get_TotalCount()
{
	return _TotalCount;
}

template<typename T>
inline DWORD CLockFreeMmemoryPool<T>::Get_UseCount()
{
	return _UseCount;
}

template<typename T>
inline T* CLockFreeMmemoryPool<T>::Alloc(bool PlacementNew)
{
	Node*		NewNode = nullptr;

	LINENODE	TopNode;

	T*			ReturnValue;

	bool		Placement = false;

	InterlockedIncrement(&_UseCount);

	if (_TotalCount <= _UseCount)
	{
		char* TempMemory = (char*)malloc(sizeof(BYTE) + sizeof(Node) + sizeof(T));

		*TempMemory = NODE_START;
		NewNode = (Node*)(TempMemory + sizeof(BYTE));

		InterlockedIncrement(&_TotalCount);
		Placement = true;
	}

	else
	{
		LONG64		Count = InterlockedDecrement64(&_Count);

		do
		{
			TopNode._Node = _Top->_Node;
			TopNode._Index = _Top->_Index;

		} while (!InterlockedCompareExchange128((LONG64*)_Top, (LONG64)Count, (LONG64)_Top->_Node->_NextNode, (LONG64*)&TopNode));

		NewNode = TopNode._Node;
	}

	ReturnValue = (T*)((char*)NewNode + sizeof(Node));

	if (true == Placement)
	{
		new (ReturnValue) T;
	}

	return ReturnValue;
}

template<typename T>
inline void CLockFreeMmemoryPool<T>::Free(T* Data)
{
	LINENODE	TopNode;

	char		StartCode = *((char*)Data - sizeof(BYTE) - sizeof(Node));
	
	if (NODE_START != StartCode)
	{
		CCrashDump::Crash();
	}

	Node*		FreeNode = (Node*)((char*)Data - sizeof(Node));

	LONG64		Count = InterlockedIncrement64(&_Count);

	do
	{
		TopNode._Node = _Top->_Node;
		TopNode._Index = _Top->_Index;

		FreeNode->_NextNode = _Top->_Node;
	} while (!InterlockedCompareExchange128((LONG64*)_Top, (LONG64)Count, (LONG64)FreeNode, (LONG64*)&TopNode));

	InterlockedDecrement(&_UseCount);
}

template<typename T>
inline void CLockFreeMmemoryPool<T>::Clear()
{
	Node* Temp = _Top->_Node;

	for (;;)
	{
		if (nullptr == Temp)
		{
			break;
		}

		Temp = Temp->_NextNode;
	}
}

template<typename T>
inline void CLockFreeMmemoryPool<T>::Release()
{
	if (nullptr == _Top->_Node)
	{
		return;
	}

	Node* Temp = (Node*)((char*)_Top->_Node - sizeof(char));
	Node* TempNextNode = nullptr;

	for (;;)
	{
		if (nullptr == Temp)
		{
			break;
		}

		TempNextNode = Temp->_NextNode;

		delete Temp;
		Temp = nullptr;

		Temp = TempNextNode - sizeof(char);
	}

	_aligned_free(_Top);
}

#endif // !LockFreeMemoryPool_H__