#include "framework.h"
#include "WindowClass.hpp" 

using namespace Gdiplus;
extern Image* image;

LRESULT CircleWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // ���׶�� �׸���
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255)); // �Ķ���
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Ellipse(hdc, 0, 0, 100, 100); // ���׶�� ũ��
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);

        EndPaint(m_hwnd, &ps);
    }
                 break;

    case WM_LBUTTONDOWN:
        MessageBox(m_hwnd, L"���׶�̰� Ŭ���Ǿ����ϴ�!", L"�̺�Ʈ �߻�", MB_OK);
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
        // �Է� ĭ (Edit Control) ����
        hEdit = CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            10, 500, 830, 90,
            m_hwnd, (HMENU)1001,
            GetModuleHandle(NULL), nullptr
        );

        // ��ư ����
        hButton = CreateWindowEx(
            0, L"BUTTON", L"SEND",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            850, 500, 90, 90,
            m_hwnd, (HMENU)0001, GetModuleHandle(NULL), nullptr
        );

        //�� �亯
        hAnswer = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"LLM�� �亯�� ���⿡ ǥ�õ˴ϴ�",
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

        // ����� ������� ä���
        HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        FillRect(hdc, &ps.rcPaint, whiteBrush);

        // GDI+ Graphics ��ü ����
        Graphics graphics(hdc);

        // �̹��� �׸���
        if (image)
        {
            RECT clientRect;
            GetClientRect(m_hwnd, &clientRect);

            int img_region_width = 950;
            int img_region_height = 350;

            // �̹��� ũ�� ��������
            int imageWidth = image->GetWidth();
            int imageHeight = image->GetHeight();

            // �߾� ��ǥ ���
            int x = (img_region_width - imageWidth) / 2;
            int y = (img_region_height - imageHeight) / 2;

            // �̹��� �׸���
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