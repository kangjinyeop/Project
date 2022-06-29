#ifndef CUSTOM_TIME_MANAGER_H__
#define CUSTOM_TIME_MANAGER_H__

#include "Engine_Macro.h"
#include "CustomTime.h"

BEGIN(Engine)

class ENGINE_DLL CCustomTimeManager
{
	DECLARE_SINGLETON(CCustomTimeManager)
private:
	explicit CCustomTimeManager();
public:
	~CCustomTimeManager();
public:
	void InsertTime(const TCHAR* TimeName);
	void DeleteTime(const TCHAR* TimeName);
	void BeginTime(const TCHAR* TimeName);
	void EndTime(const TCHAR* TimeName);
private:
	void FindTime(CCustomTime* OutTime);
	void Release();
private:
	unordered_map<const TCHAR*, CCustomTime*>			_UnMapTime;
	typedef unordered_map<const TCHAR*, CCustomTime*>	UNMAPTIME;
};

END

#endif // !CUSTOM_TIME_MANAGER_H__