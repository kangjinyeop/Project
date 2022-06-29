#include "stdafx.h"
#include "Device9.h"

USING(Engine)

IMPLEMENT_SINGLETON(CDevice9)

CDevice9::CDevice9()
	:	_SDK(NULL), _Device9(NULL),
		_WindowX(0), _WindowY(0), _WindowMode(MODE_WIN)
{
}

CDevice9::~CDevice9()
{
	Release();
}

LPDIRECT3DDEVICE9 CDevice9::GetDevice9() const
{
	return _Device9;
}

void CDevice9::SetSamplerState(_D3DSAMPLERSTATETYPE Type, DWORD Flag)
{
	if (D3DERR_INVALIDCALL == _Device9->SetSamplerState(0, Type, Flag))
	{
		CCrashDump::Crash();
	}
}

void CDevice9::SetRenderState(_D3DRENDERSTATETYPE Type, DWORD Flag)
{
	if (D3DERR_INVALIDCALL == _Device9->SetRenderState(Type, Flag))
	{
		CCrashDump::Crash();
	}
}

void CDevice9::SetTransform(_D3DTRANSFORMSTATETYPE Type, const D3DXMATRIX * Matrix)
{
	if (D3DERR_INVALIDCALL == _Device9->SetTransform(Type, Matrix))
	{
		CCrashDump::Crash();
	}
}

HRESULT CDevice9::Initialize(HWND hWnd)
{
	LoadSetUP();

	// m_pSDK객체를 생성한다.	
	_SDK = Direct3DCreate9(D3D_SDK_VERSION);

	// 장치의 성능을 조사하자.
	D3DCAPS9		CapsInfo;
	ZeroMemory(&CapsInfo, sizeof(D3DCAPS9));

	if (FAILED(_SDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &CapsInfo)))
	{
		CLog::GetInstance()->CreateLogFile(Engine::CLog::LOG_LEVEL_ERROR, Engine::CLog::LOG_MODE_TEXT, L"DirectX9 Device Info Fail.txt", NULL);
		CCrashDump::Crash();

		return E_FAIL;
	}

	DWORD		dwBehaviorFlag = 0;

	// 위에서 조사한 정보를 바탕으로 내 장치의 동작 방식을 결정한다.
	if (CapsInfo.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		dwBehaviorFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
		
	else
	{
		dwBehaviorFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}		

	dwBehaviorFlag |= D3DCREATE_MULTITHREADED;

	D3DPRESENT_PARAMETERS		d3dparameters;
	ZeroMemory(&d3dparameters, sizeof(d3dparameters));

	d3dparameters.BackBufferWidth = _WindowX;
	d3dparameters.BackBufferHeight = _WindowY;
	d3dparameters.BackBufferCount = 1;
	d3dparameters.BackBufferFormat = D3DFMT_A8R8G8B8;

	d3dparameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dparameters.MultiSampleQuality = 0;

	d3dparameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dparameters.hDeviceWindow = hWnd;
	d3dparameters.Windowed = _WindowMode;

	d3dparameters.EnableAutoDepthStencil = TRUE;
	d3dparameters.AutoDepthStencilFormat = D3DFMT_D24S8;

	// 전체화면에 관련된 속성
	d3dparameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dparameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// 장치를 생성한다.
	if (FAILED(_SDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, dwBehaviorFlag
		, &d3dparameters, &_Device9)))
	{
		CLog::GetInstance()->CreateLogFile(CLog::LOG_LEVEL_ERROR, CLog::LOG_MODE_TEXT, L"DirectX9 Create Device Fail.txt", NULL);

		CCrashDump::Crash();

		return E_FAIL;
	}

	return S_OK;
}

void CDevice9::LoadSetUP()
{
	CTextParser		TextParser;
	TextParser.LoadFile(L"../Data/Device9SetUP.txt");

	TextParser.ObjectData(L"SetUP", L"WindowSizeX", &_WindowX, CTextParser::TYPE_UINT);
	TextParser.ObjectData(L"SetUP", L"WindowSizeY", &_WindowY, CTextParser::TYPE_UINT);

	TCHAR*	WindowMode;
	TextParser.ObjectString(L"SetUP", L"WindowSizeY", &WindowMode);

	if (L"MODE_FULL" == WindowMode)
	{
		_WindowMode = MODE_FULL;
	}

	delete[] WindowMode;
	WindowMode = nullptr;
}

void CDevice9::Release()
{
	if (nullptr != _SDK)
	{
		Safe_Release(_SDK);
	}

	if (nullptr != _Device9)
	{
		Safe_Release(_Device9);
	}
}