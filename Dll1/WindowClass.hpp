#pragma once
#include <windows.h>
#include "Graph.hpp"

template <class DERIVED_TYPE>
class BaseWindow
{
public:
    HWND m_hwnd;
    PCWSTR class_name;
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL), class_name(nullptr) { }

    virtual BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    )
    {
        WNDCLASS wc = { 0 };

        wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = ClassName();
        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );

        return (m_hwnd ? TRUE : FALSE);
    }

protected:
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    virtual PCWSTR  ClassName() const = 0;
};

class PanelWindow : public BaseWindow<PanelWindow> {
public:
    int width, height;
   
public:
    PanelWindow() { }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    PCWSTR ClassName() const { return L"CustomPanel"; }

    BOOL Create(PCWSTR lpWindowName,
        DWORD dwStyle, DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    );
};

class MainWindow : public BaseWindow<MainWindow> {
public:
    HWND hEdit, hButton, hAnswer;
public:
    MainWindow() {
        hEdit = nullptr;
        hButton = nullptr;
        hAnswer = nullptr;
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    PCWSTR ClassName() const { return L"AI_Assistance"; }
    BOOL Create(PCWSTR lpWindowName,
        DWORD dwStyle, DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    );

    bool RenderGraph(Graph&& graph);
};

extern PanelWindow panel;
class CircleWindow {
public:
    static std::vector<HWND> nodes;
public:
    CircleWindow() { }

    static LRESULT CALLBACK NodeProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg)
        {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 165, 0)); // 빨간색 원
            SelectObject(hdc, hBrush);

            RECT client;
            GetClientRect(hwnd, &client);
            int centX = (client.right + client.left) / 2;
            int centY = (client.bottom + client.top) / 2;
            int radX = (client.right - client.left) / 2;
            int radY = (client.bottom - client.top) / 2;

            Ellipse(hdc, centX - radX, centY - radY, centX + radX, centY + radY);
            DeleteObject(hBrush);

            // 노드에 글자 쓰기
            Agnode_t* node = reinterpret_cast<Agnode_t*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            std::string name = agnameof(node);

            int fontSize = static_cast<int>(radX * 0.5);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));

            HFONT hFont = CreateFont(
                fontSize, 0, 0, 0, FALSE, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial")
            );
            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
            SIZE size;

            GetTextExtentPoint32A(hdc, name.c_str(), name.length(), &size);
            int text_x = centX - (size.cx / 2);
            int text_y = centY - (size.cy / 2);
            TextOutA(hdc, text_x, text_y, name.c_str(), name.length());

            EndPaint(hwnd, &ps);
        }
        break;
        case WM_LBUTTONDOWN:
            MessageBox(hwnd, L"자식 원 컨트롤 클릭됨!", L"클릭 이벤트", MB_OK);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }
};