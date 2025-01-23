#pragma once
#include <windows.h>

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

    BaseWindow() : m_hwnd(NULL) { }

    BOOL Create(
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
        wc.lpszClassName = class_name;

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle, class_name, lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
        );

        return (m_hwnd ? TRUE : FALSE);
    }

    BOOL Create(
        PCWSTR lpWindowName,
        DWORD dwStyle,
        HINSTANCE hInstance,
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
        wc.hInstance = hInstance;
        wc.lpszClassName = class_name;

        RegisterClass(&wc);

        m_hwnd = CreateWindowEx(
            dwExStyle, class_name, lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, hInstance, this
        );

        return (m_hwnd ? TRUE : FALSE);
    }

protected:
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

class MainWindow : public BaseWindow<MainWindow> {
public:
    HWND hEdit, hButton, hAnswer;
public:
    MainWindow(PCWSTR name) {
        class_name = name;
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class CircleWindow : public BaseWindow<CircleWindow> {
public:

public:
    CircleWindow() {
        class_name = nullptr;
    }

    CircleWindow(PCWSTR name) {
        class_name = name;
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};