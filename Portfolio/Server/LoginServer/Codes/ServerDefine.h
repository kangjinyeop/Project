#ifndef SERVER_DEFINE_H__
#define SERVER_DEFINE_H__

enum OverlappedState
{
	OVERLAPPED_SEND = 0,
	OVERLAPPED_RECV = 1,
	OVERLAPPED_END = 2,
};

typedef struct tagOverlapped
{
	WSAOVERLAPPED		_Overlapped;
	OverlappedState		_State;
}NEWOVERLAPPED;

#endif // !SERVER_DEFINE_H__