#include "stdafx.h"
#include "Device11.h"

USING(Engine)

IMPLEMENT_SINGLETON(CDevice11)

CDevice11::CDevice11()
	: _DirectXVersion(9),	_MonitorSizeX(0),	_MonitorSizeY(0)
{

}

CDevice11::~CDevice11()
{
	Release();
}

HRESULT CDevice11::Initialize(const TCHAR* Path)
{
	LoadSetup(Path);

#if defined(DEBUG) || defined(_DEBUG)  
	_Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//D3D11_CREATE_DEVICE_DEBUG
	HRESULT Hresult = D3D11CreateDevice(_Adapter, _DriverType, _Software, _Flags, NULL,
		0, D3D11_SDK_VERSION, &_Device11, &_FeatureLevel, &_DeviceContext);
	
	return E_NOTIMPL;
}

void CDevice11::LoadSetup(const TCHAR * Path)
{
	/*CTextParser parser;

	parser.LoadFile(Path);

	parser.SelectObject(L"SetUP");

	parser.SelectString(L"Adapter", &_Port, CTextParser::TYPE_short);

	parser.SelectString(L"DriverType", _IP, MAX_IP_SIZE);

	parser.SelectString(L"Software", &_WorkerThreadCount, CTextParser::TYPE_INT);

	parser.SelectData(L"Flags", &_ActiveWorkerThreadCount, CTextParser::TYPE_INT);

	parser.SelectString(L"WindowMode", &_SendThreadCount, CTextParser::TYPE_INT);

	parser.SelectData(L"WindowSizeX", &_AuthThreadCount, CTextParser::TYPE_INT);

	parser.SelectData(L"WindowSizeY", &_Packet_Code, CTextParser::TYPE_INT);*/
}

void CDevice11::Release()
{
	if (nullptr != _Device11)
	{
		Safe_Release(_Device11);
	}
	
	if (nullptr != _DeviceContext)
	{
		_DeviceContext->ClearState();
		Safe_Release(_DeviceContext);
	}
}