#include "framework.h"
#include "WindowClass.hpp" 

using namespace Gdiplus;
extern Image* image;

LRESULT CircleWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // 동그라미 그리기
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255)); // 파란색
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Ellipse(hdc, 0, 0, 100, 100); // 동그라미 크기
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);

        EndPaint(m_hwnd, &ps);
    }
                 break;

    case WM_LBUTTONDOWN:
        MessageBox(m_hwnd, L"동그라미가 클릭되었습니다!", L"이벤트 발생", MB_OK);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return 0;
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
            m_hwnd, (HMENU)0001, GetModuleHandle(NULL), nullptr
        );

        //모델 답변
        hAnswer = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"LLM의 답변이 여기에 표시됩니다",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            10, 350, 930, 140,
            m_hwnd, (HMENU)1002,
            GetModuleHandle(NULL), nullptr);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            //g_callback();
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // 배경을 흰색으로 채우기
        HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        FillRect(hdc, &ps.rcPaint, whiteBrush);

        // GDI+ Graphics 객체 생성
        Graphics graphics(hdc);

        // 이미지 그리기
        if (image)
        {
            RECT clientRect;
            GetClientRect(m_hwnd, &clientRect);

            int img_region_width = 950;
            int img_region_height = 350;

            // 이미지 크기 가져오기
            int imageWidth = image->GetWidth();
            int imageHeight = image->GetHeight();

            // 중앙 좌표 계산
            int x = (img_region_width - imageWidth) / 2;
            int y = (img_region_height - imageHeight) / 2;

            // 이미지 그리기
            graphics.DrawImage(image, x, y, imageWidth, imageHeight);
        }

        EndPaint(m_hwnd, &ps);
        break;
    }
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