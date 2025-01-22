#pragma once
#include "pch.h"

// ����� ���� �г� ������ ���ν���
LRESULT CALLBACK PanelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // �г��� ����� ������� ä���
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

// �гο� ������ Ŭ���� ���
void RegisterPanelClass(HINSTANCE hInstance) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = PanelProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("GraphViewer");

    RegisterClass(&wc);
}