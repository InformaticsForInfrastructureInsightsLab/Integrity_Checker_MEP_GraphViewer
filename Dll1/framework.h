#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����
#include <windows.h>
#include <string>
#include "json.hpp"

#include <ole2.h>

#pragma comment (lib, "gdiplus.lib")
#include <gdiplus.h>

// ������ Ŀ���� ���� �޽���
#define WM_UPDATE_GRAPH (WM_USER+1)
#define WM_UPDATE_CHAT (WM_USER+2)