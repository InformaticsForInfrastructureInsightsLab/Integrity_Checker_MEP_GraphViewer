#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <string>
#include "json.hpp"

#include <ole2.h>

#pragma comment (lib, "gdiplus.lib")
#include <gdiplus.h>

// 윈도우 커스텀 유저 메시지
#define WM_UPDATE_GRAPH (WM_USER+1)
#define WM_UPDATE_CHAT (WM_USER+2)