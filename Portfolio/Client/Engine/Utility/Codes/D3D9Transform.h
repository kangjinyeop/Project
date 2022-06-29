#ifndef D3D9_TRANSFORM_H__
#define D3D9_TRANSFORM_H__

#include "Engine_Macro.h"

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CD3D9Transform : public CComponent
{
public:
	explicit CD3D9Transform(const D3DXVECTOR3* pGlobalLook);
	virtual ~CD3D9Transform(void);
};

END

#endif // !D3D9_TRANSFORM_H__