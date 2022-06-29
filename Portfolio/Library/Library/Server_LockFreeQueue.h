#ifndef SERVER_LOCKFREE_QUEUE_H__
#define SERVER_LOCKFREE_QUEUE_H__

#include "Server_LockFreeMemoryPool.h"

template <typename T>
class CLockFreeQueue
{
public:
	typedef struct tagNode
	{
		tagNode*	_NextNode;
		T			_Data;		
	}Node;
public:
	CLockFreeQueue();
	~CLockFreeQueue();
public:
	DWORD						Get_Size();
	bool						Empty();
	DWORD						Get_TotalCount();
	DWORD						Get_UseCount();
public:
	void						Enqueue(T* Data);						// Tail쪽에 붙는다.
	void						Dequeue(T* Data);						// Head에서 부터 제거
	bool						Peek(T * OutData, void* NextPointer);
	void						Clear();
private:
	CLockFreeMmemoryPool<Node>	_MemoryPool;
private:
	Node*						_HeadNode;
	Node*						_TailNode;
private:
	DWORD						_Size;
};

template<typename T>
inline CLockFreeQueue<T>::CLockFreeQueue()
	: _Size(0)
{
	_HeadNode = _MemoryPool.Alloc();
	_HeadNode->_NextNode = nullptr;
	_TailNode = _HeadNode;
}

template<typename T>
inline CLockFreeQueue<T>::~CLockFreeQueue()
{
	_MemoryPool.Release();
}

template<typename T>
inline DWORD CLockFreeQueue<T>::Get_Size()
{
	return _Size;
}

template<typename T>
inline bool CLockFreeQueue<T>::Empty()
{
	Node* Head = _HeadNode;
	Node* Tail = _TailNode;

	if (Head == Tail)
	{
		return true;
	}

	if (nullptr == Head->_NextNode)
	{
		return true;
	}

	return false;
}

template<typename T>
inline DWORD CLockFreeQueue<T>::Get_TotalCount()
{
	return _MemoryPool.Get_TotalCount();
}

template<typename T>
inline DWORD CLockFreeQueue<T>::Get_UseCount()
{
	return _MemoryPool.Get_UseCount();
}

template<typename T>
inline void CLockFreeQueue<T>::Clear()
{
	for (DWORD Index = 0; Index < _Size; ++Index)
	{
		Node* TempHeadNode = _HeadNode;
		Node* TempTailNode = _TailNode;
		Node* TempNextNode = _TailNode->_NextNode;

		/*
		Tail의 NextPointer가 NULL이 아닌경우 맨끝이 아니다.
		이러한 경우 이동해야 한다.
		*/
		if (nullptr != TempNextNode)
		{
			InterlockedCompareExchangePointer((PVOID*)&_TailNode, TempNextNode, TempTailNode);
		}

		for (;;)
		{
			TempHeadNode = _HeadNode;
			TempNextNode = _HeadNode->_NextNode;

			// 더미노드이다.
			if (nullptr == TempNextNode)
			{
				break;
			}

			if (TempHeadNode == InterlockedCompareExchangePointer((PVOID*)&_HeadNode, TempNextNode, TempHeadNode))
			{
				_MemoryPool.Free(TempHeadNode);
				break;
			}
		}
	}

	_Size = 0;
}

template<typename T>
inline void CLockFreeQueue<T>::Enqueue(T* Data)
{
	DWORD	Size = _Size;

	// 새로운 노드 생성
	Node* NewNode = (Node*)_MemoryPool.Alloc();
	NewNode->_Data = *Data;
	NewNode->_NextNode = nullptr;

	Node* TempTailNode = _TailNode;
	Node* TempNextNode = _TailNode->_NextNode;

	/*
	Tail의 NextPointer가 NULL이 아닌경우 맨끝이 아니다.
	이러한 경우 이동해야 한다.
	*/
	if (nullptr != TempNextNode)
	{
		InterlockedCompareExchangePointer((PVOID*)&_TailNode, TempNextNode, TempTailNode);
	}

	/*
	두번의 작업
	1. 현재 마지막 노드의 next pointer에다가 새로운 노드 연결
	 1) Next가 null인지 확인
	2. Tail의 위치 이동 <- 한번만 수행, 실패해도 된다.
	*/
	for (;;)
	{
		TempTailNode = _TailNode;
		TempNextNode = _TailNode->_NextNode;

		if (nullptr == TempNextNode)
		{
			if (TempNextNode == InterlockedCompareExchangePointer((PVOID*)&TempTailNode->_NextNode, NewNode, TempNextNode))
			{
				InterlockedCompareExchangePointer((PVOID*)&_TailNode, NewNode, TempTailNode);
				break;
			}
		}
	}

	InterlockedIncrement(&_Size);
}

template<typename T>
inline void CLockFreeQueue<T>::Dequeue(T* Data)
{
	DWORD	Size = _Size;

	if (0 >= Size)
	{
		return;
	}

	Node* TempHeadNode = _HeadNode;
	Node* TempTailNode = _TailNode;
	Node* TempNextNode = _TailNode->_NextNode;

	/*
	Tail의 NextPointer가 NULL이 아닌경우 맨끝이 아니다.
	이러한 경우 이동해야 한다.
	*/
	if (nullptr != TempNextNode)
	{
		InterlockedCompareExchangePointer((PVOID*)&_TailNode, TempNextNode, TempTailNode);
	}

	for (;;)
	{
		TempHeadNode = _HeadNode;
		TempNextNode = _HeadNode->_NextNode;

		// 더미노드이다.
		if (nullptr == TempNextNode)
		{
			break;
		}

		if (TempHeadNode == InterlockedCompareExchangePointer((PVOID*)&_HeadNode, TempNextNode, TempHeadNode))
		{
			*Data = TempNextNode->_Data;
			InterlockedDecrement(&_Size);
			_MemoryPool.Free(TempHeadNode);
			break;
		}
	}
}
template<typename T>
inline bool CLockFreeQueue<T>::Peek(T * OutData, void* NextPointer)
{
	Node* TempNode = (Node*)NextPointer;

	if (nullptr == NextPointer)
	{
		*OutData = _HeadNode->_NextNode->_Data;
		NextPointer = _HeadNode->_NextNode->_NextNode;
	}	

	else
	{
		*OutData = TempNode->_NextNode->_Data;
		NextPointer = TempNode->_NextNode->_NextNode;
	}

	return true;
}
#endif // !SERVER_LOCKFREE_QUEUE_H__