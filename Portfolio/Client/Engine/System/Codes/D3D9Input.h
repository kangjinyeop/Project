#ifndef D3D9_INPUT_H__
#define D3D9_INPUT_H__

BEGIN(Engine)

class ENGINE_DLL CD3D9Input
{
	DECLARE_SINGLETON(CD3D9Input)
private:
#define STATE_MAX 256
private:
	explicit CD3D9Input();
public:
	~CD3D9Input();
public:
	void Initalize(HINSTANCE Instance, HWND Hwnd);
private:
	void Initalize_KeyBoard(HWND hWnd);
	void Initalize_Mouse(HWND hWnd);
	void Release();
private:
	LPDIRECTINPUT8				_Input;
	LPDIRECTINPUTDEVICE8		_KeyBoard;
	BYTE						_KeyState[STATE_MAX];
	LPDIRECTINPUTDEVICE8		_Mouse;
	DIMOUSESTATE				_MouseState;
};

END

#endif // !D3D9_INPUT_H__