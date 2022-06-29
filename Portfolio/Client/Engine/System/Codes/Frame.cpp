#include "stdafx.h"
#include "Frame.h"

USING(Engine)

CFrame::CFrame()
	:	_CallCount(0), _FrameRate(0), _Frame(0),
		_IntervalFrame(0)
{
	QueryPerformanceFrequency(&_Frequency);
}

CFrame::~CFrame()
{
	Release();
}

bool CFrame::RenderPause(const int Frame)
{
	return false;
}

HRESULT CFrame::Initalize(const int ObjectiveFrame)
{
	_FrameRate = 1 / static_cast<double>(ObjectiveFrame);

	return S_OK;
}

void CFrame::BeginTime()
{
	QueryPerformanceCounter(&_BeginTime);
}

void CFrame::EndTime()
{
	QueryPerformanceCounter(&_EndTime);

	UINT64 ElapsedTime = _EndTime.QuadPart - _BeginTime.QuadPart;
	double DuringTime = static_cast<double>(ElapsedTime) / static_cast<double>(_Frequency.QuadPart);
}

void CFrame::Update(const double UpdateFrame)
{
	if (0 == UpdateFrame)
	{
		return;
	}

	_Frame += UpdateFrame;

	if (_Frame > 1.f)
	{
		_Frame = 0;
		_CallCount = 0;
	}

	++_CallCount;
}

void CFrame::Release()
{
}
