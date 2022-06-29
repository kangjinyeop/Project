#ifndef SERVER_QUEUE_H__
#define SERVER_QUEUE_H__

#include "Server_CrashDump.h"

template <typename T>
class CCustomQueue
{
private:
#define BLOCK 1
public:
	CCustomQueue();
	~CCustomQueue();
public:
	INT			Get_Size();
	INT			Get_Use();
public:
	void		MoveFront(INT Size);
	void		MoveRear(INT Size);
public:
	void		Initialize(INT MaxSize);
	void		Enqueue(T Data);
	void		Dequeue(T* Data);
	T			Peek(INT Size = 0);
	void		Clear();
private:
	T*			_Buffer;
	INT			_Size;
	INT			_Front;
	INT			_Rear;
};

template<typename T>
inline CCustomQueue<T>::CCustomQueue()
	:	_Buffer(nullptr), _Size(0), _Front(0),
		_Rear(0)
{
}

template<typename T>
inline CCustomQueue<T>::~CCustomQueue()
{
	delete[] _Buffer;
	_Buffer = nullptr;
}

template<typename T>
inline INT CCustomQueue<T>::Get_Size()
{
	return _Size - 1;
}

template<typename T>
inline INT CCustomQueue<T>::Get_Use()
{
	return (_Rear - _Front + _Size) % _Size;
}

template<typename T>
inline void CCustomQueue<T>::MoveFront(INT Size)
{
	INT Front = _Front;

	_Front = (Front + Size) % _Size;
}

template<typename T>
inline void CCustomQueue<T>::MoveRear(INT Size)
{
	INT Rear = _Rear;

	_Rear = (Rear + Size) % _Size;
}

template<typename T>
inline void CCustomQueue<T>::Initialize(INT MaxSize)
{
	_Size = MaxSize + 1;
	_Buffer = new T[_Size];

	if (nullptr == _Buffer)
	{
		CCrashDump::Crash();
	}

	memset(_Buffer, 0, sizeof(T) * _Size);
}

template<typename T>
inline void CCustomQueue<T>::Enqueue(T Data)
{
	INT Rear = _Rear;

	Rear = (Rear + 1) % _Size;

	_Buffer[Rear] = Data;

	_Rear = Rear;
}

template<typename T>
inline void CCustomQueue<T>::Dequeue(T * Data)
{
	INT Front = _Front;

	Front = (Front + 1) % _Size;

	*Data = _Buffer[Front];

	_Front = Front;
}

template<typename T>
inline T CCustomQueue<T>::Peek(INT Size)
{	
	int TempFront = (_Front + 1 + Size) % _Size;

	return _Buffer[TempFront];
}

template<typename T>
inline void CCustomQueue<T>::Clear()
{
	_Front = 0;
	_Rear = 0;
}

#endif // !SERVER_QUEUE_H__