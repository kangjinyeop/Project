#ifndef DEVICE9_H__
#define DEVICE9_H__

BEGIN(Engine)

class ENGINE_DLL CDevice9
{
	DECLARE_SINGLETON(CDevice9)
private:
	enum WINMODE
	{ 
		MODE_FULL	= 0,
		MODE_WIN	= 1,
		MODE_END	= 2,
	}; 
private:
	explicit CDevice9(void);
public:
	~CDevice9();
public:
	LPDIRECT3DDEVICE9			GetDevice9() const;
public:
	void						SetSamplerState(_D3DSAMPLERSTATETYPE Type, DWORD Flag);
	void						SetRenderState(_D3DRENDERSTATETYPE Type, DWORD Flag);
	void						SetTransform(_D3DTRANSFORMSTATETYPE Type, const D3DXMATRIX* Matrix);
public:
	HRESULT	Initialize(HWND hWnd);
private:
	void						LoadSetUP();
	void						Release();
private:
	LPDIRECT3D9					_SDK;
	LPDIRECT3DDEVICE9			_Device9;
	UINT						_WindowX;
	UINT						_WindowY;
	WINMODE						_WindowMode;
};

END

#endif // !DEVICE9_H__