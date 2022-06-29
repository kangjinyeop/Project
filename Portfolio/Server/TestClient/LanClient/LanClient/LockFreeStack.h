#ifndef LockFreeStack_H__
#define LockFreeStack_H__

#include "LockFreeMemoryPool.h"

template <typename T>
class CLockFreeStack
{
private:
	typedef struct tagNode
	{
		tagNode*	_NextNode;
		T			_Data;
	}Node;
	typedef struct tagLineNode
	{
		Node*		_Node;
		LONG64		_Index;
	}LINENODE;
public:
	CLockFreeStack();
	~CLockFreeStack();
public:
	LONG64		Get_Count();
	DWORD		Get_AllocCount();
	DWORD		Get_OwnCount();
	DWORD		Get_UseCount();
public:
	void		Push(T Data);
	void		Pop(T* Data);
	void		Clear();
private:
	CLockFreeMmemoryPool<Node>	_MemoryPool;
private:
	LINENODE*					_Top;
	LONG64						_Count;		// 개수
};

template <typename T>
inline CLockFreeStack<T>::CLockFreeStack()
	: _Count(0)
{
	_Top = (LINENODE*)_aligned_malloc(sizeof(LINENODE), 16);
	memset(_Top, 0, sizeof(LINENODE));
}

template <typename T>
inline CLockFreeStack<T>::~CLockFreeStack()
{
	_MemoryPool.Release();
	_aligned_free(_Top);
}

template <typename T>
inline LONG64 CLockFreeStack<T>::Get_Count()
{
	return _Count;
}

template <typename T>
inline DWORD CLockFreeStack<T>::Get_AllocCount()
{
	return _MemoryPool.Get_AllocCount();
}

template <typename T>
inline DWORD CLockFreeStack<T>::Get_OwnCount()
{
	return _MemoryPool.Get_OwnCount();
}

template <typename T>
inline DWORD CLockFreeStack<T>::Get_UseCount()
{
	return _MemoryPool.Get_UseCount();
}

template <typename T>
inline void CLockFreeStack<T>::Push(T Data)
{
	LINENODE	TopNode;
	Node*		NewNode = _MemoryPool.Alloc();	

	LONG64		Count = InterlockedIncrement64(&_Count);

	do
	{
		TopNode = *_Top;

		NewNode->_NextNode = TopNode._Node;
		NewNode->_Data = Data;

	} while (!InterlockedCompareExchange128((LONG64*)_Top, (LONG64)Count, (LONG64)NewNode, (LONG64*)&TopNode));
}

template <typename T>
inline typename void CLockFreeStack<T>::Pop(T* Data)
{
	LONG64 Count = _Count;

	if (Count <= 0)
	{
		return;
	}

	LINENODE	TopNode;	

	Node*		FreeNode = nullptr;

	_Count = InterlockedDecrement64(&Count);

	do
	{
		/*
		현재 Top 복사
		*/
		TopNode._Node = _Top->_Node;
		TopNode._Index = _Top->_Index;

		FreeNode = TopNode._Node;
		FreeNode->_Data = TopNode._Node->_Data;

	} while (!InterlockedCompareExchange128((LONG64*)_Top, (LONG64)Count, (LONG64)FreeNode->_NextNode, (LONG64*)&TopNode));

	*Data = FreeNode->_Data;

	_MemoryPool.Free(FreeNode);
}

template<typename T>
inline void CLockFreeStack<T>::Clear()
{
	_MemoryPool.Clear();
	memset(_Top, 0, sizeof(LINENODE));
}

#endif // !LockFreeStack_H__