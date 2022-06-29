#include "stdafx.h"
#include "Engine_CustomTime.h"

USING(Engine)

CCustomTime::CCustomTime()
{
	QueryPerformanceFrequency(&_Frequency);
}

CCustomTime::~CCustomTime()
{
	Release();
}

void CCustomTime::BeginTime()
{
	QueryPerformanceCounter(&_BeginTime);
}

void CCustomTime::EndTime()
{
	QueryPerformanceCounter(&_EndTime);

	UINT64 ElapsedTime = _EndTime.QuadPart - _BeginTime.QuadPart;
	double DuringTime = static_cast<double>(ElapsedTime) / static_cast<double>(_Frequency.QuadPart);
}

void CCustomTime::Release()
{
}