#pragma once
#include "pch.h"

// 사용자 정의 패널 윈도우 프로시저
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 패널의 배경을 흰색으로 채우기
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);

        EndPaint(hwnd, &ps);
        break;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

// 패널용 윈도우 클래스 등록
void RegisterPanelClass(HINSTANCE hInstance) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = PanelProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("GraphViewer");

    RegisterClass(&wc);
}