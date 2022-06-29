#ifndef D3D_FONT_MANAGER_H__
#define D3D_FONT_MANAGER_H__

#include "Engine_Macro.h"

BEGIN(Engine)

class ENGINE_DLL CD3DFontManager
{
private:
	explicit CD3DFontManager();
public:
	~CD3DFontManager();
private:
	void Release();
};

END

#endif // !D3D_FONT_MANAGER_H__