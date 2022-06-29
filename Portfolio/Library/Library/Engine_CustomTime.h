#ifndef ENGINE_CUSTOMTIME_H__
#define ENGINE_CUSTOMTIME_H__

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CCustomTime
{
public:
	explicit CCustomTime();
	~CCustomTime();
public:
	void BeginTime();
	void EndTime();
private:
	void Release();
private:
	LARGE_INTEGER	_Frequency;
	LARGE_INTEGER	_BeginTime;
	LARGE_INTEGER	_EndTime;	
};

END

#endif // !ENGINE_CUSTOMTIME_H__