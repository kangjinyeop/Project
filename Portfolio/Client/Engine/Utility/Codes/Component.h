#ifndef COMPONENT_H__
#define COMPONENT_H__

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CComponent
{
public:
	explicit CComponent(void);
	virtual ~CComponent(void);
public:
	virtual HRESULT Initialize(void);
	virtual void Update(const double& TimePerSec);
	virtual void D3D9Render(LPDIRECT3DDEVICE9 Device);
	virtual void D3D11Render(LPDIRECT3DDEVICE9 Device);
};

END

#endif // !COMPONENT_H__