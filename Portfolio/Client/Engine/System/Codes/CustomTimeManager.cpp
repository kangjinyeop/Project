#include "stdafx.h"
#include "CustomTimeManager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CCustomTimeManager)

CCustomTimeManager::CCustomTimeManager()
{
}

CCustomTimeManager::~CCustomTimeManager()
{
	Release();
}

void CCustomTimeManager::InsertTime(const TCHAR * TimeName)
{
}

void CCustomTimeManager::DeleteTime(const TCHAR * TimeName)
{
}

void CCustomTimeManager::BeginTime(const TCHAR * TimeName)
{
}

void CCustomTimeManager::EndTime(const TCHAR * TimeName)
{
}

void CCustomTimeManager::Release()
{
}