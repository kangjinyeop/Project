// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
// STL
#include <vector>
#include <list>
#include <map>
#include <unordered_map>

// 서버
#include <WinSock2.h>
#include <ws2tcpip.h>

#include <algorithm>
#include <ctime>
#include <cmath>
#include <Windows.h>
#include <atlstr.h>
#include <dbghelp.h>
#include <strsafe.h>

// DirectX9
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

// DirectX11
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

#include "Engine_Defines.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

// 서버
//#include "LockFreeMemoryPool.h"
//#include "StreamBuffer.h"
//#include "RingBuffer.h"
//#include "LockFreeQueue.h"
//#include "ServerDefine.h"

// Library
#include "Engine_Log.h"
#include "Engine_TextParser.h"
#include "Engine_CrashDump.h"