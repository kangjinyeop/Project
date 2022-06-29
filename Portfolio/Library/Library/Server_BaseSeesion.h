#ifndef SERVER_BASESESSION_H
#define SERVER_BASESESSION_H

#include "Server_RingBuffer.h"
#include "Server_StreamBuffer.h"
#include "Server_LockFreeQueue.h"
#include "Server_Queue.h"

class CBaseSession
{
public:
	explicit CBaseSession();
	virtual ~CBaseSession();
};

#endif // !SERVER_BASESESSION_H