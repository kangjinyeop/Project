#ifndef FRAME_H__
#define FRAME_H__

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CFrame
{
public:
	explicit CFrame();
	~CFrame();
public:
	bool RenderPause(const int Frame);
public:
	HRESULT	Initalize(const int ObjectiveFrame);
	void BeginTime();
	void EndTime();
	void Update(const double UpdateFrame);
private:
	void Release();
private:
	LARGE_INTEGER	_Frequency;
	LARGE_INTEGER	_BeginTime;
	LARGE_INTEGER	_EndTime;
	DWORD			_CallCount;
	double			_FrameRate;
	double			_Frame;
	double			_IntervalFrame;
};

END

#endif // !FRAME_H__