#include "framework.h"
#include "WindowClass.hpp" 

using namespace Gdiplus;
extern Image* image;

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

LRESULT PanelWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static float scale = 1.0f;
    static float offsetX, offsetY;

    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // ����� ������� ����
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);

        // ������ �������� ����
        SetMapMode(hdc, MM_ANISOTROPIC);
        SetWindowExtEx(hdc, 1000, 1000, NULL);
        SetViewportExtEx(hdc, (int)(1000 * scale), (int)(1000 * scale), NULL);
        SetViewportOrgEx(hdc, (int)(-offsetX * scale), (int)(-offsetY * scale), NULL);

        EndPaint(m_hwnd, &ps);
    }
    break;

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam); // ���콺 �� �̵� ��
        float oldScale = scale;

        // ���� ����
        if (delta > 0) {
            scale += 0.1f; // Ȯ��
        }
        else if (delta < 0) {
            scale -= 0.1f; // ���
            if (scale < 0.1f) scale = 0.1f; // �ּ� ���� ����
        }

        // ���콺 ��ġ�� ����
        POINT cursorPos;
        GetCursorPos(&cursorPos); // ȭ�� ��ǥ ���
        ScreenToClient(m_hwnd, &cursorPos); // Ŭ���̾�Ʈ ��ǥ�� ��ȯ

        // ���콺 ��ġ�� �������� �������� ����
        float mouseX = cursorPos.x / oldScale + offsetX; // ���� ���������� ���콺 X ��ǥ
        float mouseY = cursorPos.y / oldScale + offsetY; // ���� ���������� ���콺 Y ��ǥ

        offsetX = mouseX - cursorPos.x / scale; // ���ο� ������ ���� X�� ������
        offsetY = mouseY - cursorPos.y / scale; // ���ο� ������ ���� Y�� ������

        // ȭ�� �ٽ� �׸���
        InvalidateRect(m_hwnd, NULL, TRUE);
    }
    break;
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

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