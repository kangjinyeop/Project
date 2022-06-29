#ifndef DEVICE11_H__
#define DEVICE11_H__

#include "Engine_TextParser.h"

BEGIN(Engine)

class ENGINE_DLL CDevice11
{
	DECLARE_SINGLETON(CDevice11)
private:
	explicit CDevice11(void);
public:
	~CDevice11(void);
public:
	HRESULT					Initialize(const TCHAR* Path);
private:
	void					LoadSetup(const TCHAR* Path);
	void					Release();
private:
	int						_DirectXVersion;
	int						_MonitorSizeX;
	int						_MonitorSizeY;
// DirectX 11
private:
	ID3D11Device*			_Device11;
	ID3D11DeviceContext*	_DeviceContext;
	IDXGIAdapter*			_Adapter;
	D3D_DRIVER_TYPE			_DriverType;	
	HMODULE					_Software;
	int						_Flags;
	D3D_FEATURE_LEVEL		_FeatureLevel;
};

END

#endif // !DEVICE11_H__