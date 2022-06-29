#ifndef FREAM_MANAGER_H__
#define FREAM_MANAGER_H__

#include "Engine_Macro.h"

#include "Frame.h"

BEGIN(Engine)

class ENGINE_DLL CFrameManager
{
	DECLARE_SINGLETON(CFrameManager)
private:
	explicit CFrameManager();
public:
	~CFrameManager();
public:
	bool RenderPause(const int Frame);
public:
	HRESULT InsertFrame(const int InFrame);
	HRESULT UpdateFrame(const int UpdateFrame);
	HRESULT DeleteFrame(const int DeleteFrame);
private:
	bool FindFrame(const int SearchFrame);
private:
	void Release();
private:
	unordered_map<int, CFrame*>			_UnMapFrame;
	typedef unordered_map<int, CFrame*>	UNMAPFRAME;
};

END

#endif // !FREAM_MANAGER_H__