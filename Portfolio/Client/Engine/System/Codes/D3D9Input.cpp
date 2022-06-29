#include "stdafx.h"
#include "D3D9Input.h"

USING(Engine)

IMPLEMENT_SINGLETON(CD3D9Input)

CD3D9Input::CD3D9Input()
	: _Input(nullptr), _KeyBoard(nullptr), _Mouse(nullptr)
{
}

CD3D9Input::~CD3D9Input()
{
	Release();
}

void CD3D9Input::Initalize(HINSTANCE Instance, HWND Hwnd)
{
	if (FAILED(DirectInput8Create(Instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_Input, NULL)))
	{
		CCrashDump::Crash();		
	}

	Initalize_KeyBoard(Hwnd);
	Initalize_Mouse(Hwnd);
}

void CD3D9Input::Initalize_KeyBoard(HWND hWnd)
{
	if (FAILED(_Input->CreateDevice(GUID_SysKeyboard, &_KeyBoard, NULL)))
	{
		CCrashDump::Crash();
	}

	if (FAILED(_KeyBoard->SetDataFormat(&c_dfDIKeyboard)))
	{
		CCrashDump::Crash();
	}

	if (FAILED(_KeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		CCrashDump::Crash();
	}

	_KeyBoard->Acquire();
}

void CD3D9Input::Initalize_Mouse(HWND hWnd)
{
	if (FAILED(_Input->CreateDevice(GUID_SysKeyboard, &_Mouse, NULL)))
	{
		CCrashDump::Crash();
	}

	if (FAILED(_Mouse->SetDataFormat(&c_dfDIMouse)))
	{
		CCrashDump::Crash();
	}

	if (FAILED(_Mouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		CCrashDump::Crash();
	}

	_Mouse->Acquire();
}

void CD3D9Input::Release()
{
	_Input->Release();

	if (nullptr != _KeyBoard)
	{
		_KeyBoard->Release();
	}
	
	if (nullptr != _Mouse)
	{
		_Mouse->Release();
	}
}