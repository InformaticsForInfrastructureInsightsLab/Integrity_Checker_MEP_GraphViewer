#include "framework.h"
#include "WindowClass.hpp" 

using namespace Gdiplus;

#include "utils.hpp"
extern CallbackFunc g_callback;

extern MainWindow win;

#pragma region MainWindow
BOOL MainWindow::Create(PCWSTR lpWindowName,
    DWORD dwStyle, DWORD dwExStyle,
    int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu) {

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName();
    RegisterClass(&wc);

    RECT size = { 0, 0, 950, 600 };
    AdjustWindowRect(&size, dwStyle, FALSE);

    m_hwnd = CreateWindow(
        ClassName(), lpWindowName, dwStyle, x, y,
        size.right - size.left, size.bottom - size.top, 
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_CREATE:
        // 입력 칸 (Edit Control) 생성
        hEdit = CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            10, 500, 830, 90,
            m_hwnd, (HMENU)1001,
            GetModuleHandle(NULL), nullptr
        );

        // 버튼 생성
        hButton = CreateWindowEx(
            0, L"BUTTON", L"SEND",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            850, 500, 90, 90,
            m_hwnd, (HMENU)2001, GetModuleHandle(NULL), nullptr
        );

        // 모델 답변
        hAnswer = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"LLM의 답변이 여기에 표시됩니다",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            10, 350, 930, 140,
            m_hwnd, (HMENU)1002,
            GetModuleHandle(NULL), nullptr);

        // 그래프 패널
        panel = new PanelWindow();
        if (!panel->Create(L"GraphPanel",
            WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_NOTIFY, 0,
            0, 0, 950, 340, m_hwnd, (HMENU)3001)) {
            MessageBox(m_hwnd, L"panel create fail", L" ", MB_OK);
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 2001) {
            g_callback();
        }
        break;
    case WM_CLOSE:
        DestroyWindow(m_hwnd);
        break;
    case WM_DESTROY:
        UnregisterClass(L"MyWindowClass", GetModuleHandle(NULL));
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
#pragma endregion


#pragma region PanelWindow
extern MainWindow win;
LRESULT PanelWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static float scale = 1.0f;
    static float offsetX, offsetY;

    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // 배경을 흰색으로 지움
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);

        Ellipse(hdc, 50, 50, 150, 150);

        // 배율과 오프셋을 적용
        SetMapMode(hdc, MM_ANISOTROPIC);
        SetWindowExtEx(hdc, 1000, 1000, NULL);
        SetViewportExtEx(hdc, (int)(1000 * scale), (int)(1000 * scale), NULL);
        SetViewportOrgEx(hdc, (int)(-offsetX * scale), (int)(-offsetY * scale), NULL);

        EndPaint(m_hwnd, &ps);
        break;
    }
    case WM_LBUTTONDOWN:
        MessageBox(m_hwnd, L"panel_clicked", L" ", MB_OK);
        return 0;
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam); // 마우스 휠 이동 값
        float oldScale = scale;

        // 배율 조정
        if (delta > 0) {
            scale += 0.1f; // 확대
        }
        else if (delta < 0) {
            scale -= 0.1f; // 축소
            if (scale < 0.1f) scale = 0.1f; // 최소 배율 제한
        }

        // 마우스 위치를 얻음
        POINT cursorPos;
        GetCursorPos(&cursorPos); // 화면 좌표 얻기
        ScreenToClient(m_hwnd, &cursorPos); // 클라이언트 좌표로 변환

        // 마우스 위치를 기준으로 오프셋을 조정
        float mouseX = cursorPos.x / oldScale + offsetX; // 기존 배율에서의 마우스 X 좌표
        float mouseY = cursorPos.y / oldScale + offsetY; // 기존 배율에서의 마우스 Y 좌표

        offsetX = mouseX - cursorPos.x / scale; // 새로운 배율에 맞춘 X축 오프셋
        offsetY = mouseY - cursorPos.y / scale; // 새로운 배율에 맞춘 Y축 오프셋

        // 화면 다시 그리기
        InvalidateRect(m_hwnd, NULL, TRUE);
        MessageBox(m_hwnd, L"mouse wheel", L" ", MB_OK);
        return 0;
    }
    default:
        break;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

BOOL PanelWindow::Create(PCWSTR lpWindowName,
    DWORD dwStyle, DWORD dwExStyle,
    int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu) {

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName();
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClass(&wc);

    RECT size = { x,y,nWidth,nHeight };
    AdjustWindowRect(&size, dwStyle, FALSE);

    width = size.right - size.left;
    height = size.bottom - size.top;

    m_hwnd = CreateWindow(
        ClassName(), lpWindowName, dwStyle, 
        x, y, width, height,
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}
#pragma endregion


#pragma region CircleWindow
LRESULT CircleWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL CircleWindow::Create(PCWSTR lpWindowName,
    DWORD dwStyle, DWORD dwExStyle,
    int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu) {

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName();
    return (RegisterClass(&wc) != 0);
}
#pragma endregion