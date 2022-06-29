#include "stdafx.h"
#include "Base.h"

USING(Engine)

CBase::CBase()
	: _ReferenceCount(0)
{
}

CBase::~CBase()
{
}

_ulong CBase::IncreaseReferenceCount()
{
	++_ReferenceCount;

	return _ReferenceCount;
}

_ulong CBase::DecreaseReferenceCount()
{
	--_ReferenceCount;

	return _ReferenceCount;
}