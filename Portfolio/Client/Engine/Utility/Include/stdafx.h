// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <Windows.h>

// DirectX
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

#include "Engine_Defines.h"
#include "Engine_Macro.h"
#include "Engine_Typedef.h"

// Library
/*
#include "Engine_Log.h"
#include "Engine_TextParser.h"
#include "Engine_CrashDump.h"
*/