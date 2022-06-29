#ifndef SERVER_CUSTOMTIME_H__
#define SERVER_CUSTOMTIME_H__

class CCustomTime
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

#endif // !SERVER_CUSTOMTIME_H__