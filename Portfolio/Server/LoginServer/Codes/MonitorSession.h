#ifndef MONITOR_SEESION_H__
#define MONITOR_SEESION_H__

#include "RingBuffer.h"
#include "StreamBuffer.h"

class CMonitorSession
{
public:
	CMonitorSession();
	~CMonitorSession();
public:
	CRingBuffer		_RingBuffer;
	CStreamBuffer	_StreamBuffer;
};

#endif // !MONITOR_SEESION_H__