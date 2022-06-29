#ifndef BASE_H__
#define BASE_H__

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CBase
{
public:
	explicit CBase();
	virtual ~CBase();
public:
	UINT	IncreaseReferenceCount();
	_ulong	DecreaseReferenceCount();
protected:
	INT64  _ReferenceCount;
};

END

#endif // !BASE_H__