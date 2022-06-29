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
	void						Enqueue(T* Data);						// Tail�ʿ� �ٴ´�.
	void						Dequeue(T* Data);						// Head���� ���� ����
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
		Tail�� NextPointer�� NULL�� �ƴѰ�� �ǳ��� �ƴϴ�.
		�̷��� ��� �̵��ؾ� �Ѵ�.
		*/
		if (nullptr != TempNextNode)
		{
			InterlockedCompareExchangePointer((PVOID*)&_TailNode, TempNextNode, TempTailNode);
		}

		for (;;)
		{
			TempHeadNode = _HeadNode;
			TempNextNode = _HeadNode->_NextNode;

			// ���̳���̴�.
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

	// ���ο� ��� ����
	Node* NewNode = (Node*)_MemoryPool.Alloc();
	NewNode->_Data = *Data;
	NewNode->_NextNode = nullptr;

	Node* TempTailNode = _TailNode;
	Node* TempNextNode = _TailNode->_NextNode;

	/*
	Tail�� NextPointer�� NULL�� �ƴѰ�� �ǳ��� �ƴϴ�.
	�̷��� ��� �̵��ؾ� �Ѵ�.
	*/
	if (nullptr != TempNextNode)
	{
		InterlockedCompareExchangePointer((PVOID*)&_TailNode, TempNextNode, TempTailNode);
	}

	/*
	�ι��� �۾�
	1. ���� ������ ����� next pointer���ٰ� ���ο� ��� ����
	 1) Next�� null���� Ȯ��
	2. Tail�� ��ġ �̵� <- �ѹ��� ����, �����ص� �ȴ�.
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
	Tail�� NextPointer�� NULL�� �ƴѰ�� �ǳ��� �ƴϴ�.
	�̷��� ��� �̵��ؾ� �Ѵ�.
	*/
	if (nullptr != TempNextNode)
	{
		InterlockedCompareExchangePointer((PVOID*)&_TailNode, TempNextNode, TempTailNode);
	}

	for (;;)
	{
		TempHeadNode = _HeadNode;
		TempNextNode = _HeadNode->_NextNode;

		// ���̳���̴�.
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