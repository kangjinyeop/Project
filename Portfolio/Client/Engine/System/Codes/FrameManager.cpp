#include "stdafx.h"
#include "FrameManager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CFrameManager)

CFrameManager::CFrameManager()
{	
}

CFrameManager::~CFrameManager()
{
	Release();
}

bool CFrameManager::RenderPause(const int Frame)
{
	if (true == FindFrame(Frame))
	{
		return false;
	}

	return _UnMapFrame[UpdateFrame]->RenderPause(Frame);
}

HRESULT CFrameManager::InsertFrame(const int InFrame)
{
	if (true == FindFrame(InFrame))
	{
		return E_FAIL;
	}

	CFrame* Frame = new CFrame;

	Frame->Initalize(InFrame);

	_UnMapFrame.insert(UNMAPFRAME::value_type(InFrame, Frame));

	return S_OK;
}

HRESULT CFrameManager::UpdateFrame(const int UpdateFrame)
{
	if (false == FindFrame(UpdateFrame))
	{
		return E_FAIL;
	}

	_UnMapFrame[UpdateFrame]->Update(UpdateFrame);

	return S_OK;
}

HRESULT CFrameManager::DeleteFrame(const int DeleteFrame)
{
	if (false == FindFrame(DeleteFrame))
	{
		return E_FAIL;
	}

	delete _UnMapFrame[DeleteFrame];
	_UnMapFrame[DeleteFrame] = nullptr;

	_UnMapFrame.erase(DeleteFrame);

	return S_OK;
}

bool CFrameManager::FindFrame(const int SearchFrame)
{
	if (_UnMapFrame.end() == _UnMapFrame.find(SearchFrame))
	{
		return true;
	}

	return false;
}

void CFrameManager::Release()
{
	UNMAPFRAME::iterator	Iter_begin = _UnMapFrame.begin();
	UNMAPFRAME::iterator	Iter_end = _UnMapFrame.end();

	for (; Iter_begin != Iter_end; ++Iter_begin)
	{
		delete Iter_begin->second;
		Iter_begin->second = nullptr;
	}

	_UnMapFrame.clear();
}