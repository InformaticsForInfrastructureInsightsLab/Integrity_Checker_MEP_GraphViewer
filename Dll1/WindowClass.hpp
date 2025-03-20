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
    float offsetX, offsetY;
public:
    PanelWindow() : offsetX(0), offsetY(0) { }

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

class ChatPanelWindow : public PanelWindow {
public:
    ChatPanelWindow() { }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    PCWSTR ClassName() const { return L"STATIC"; }

    BOOL Create(PCWSTR lpWindowName,
        DWORD dwStyle, DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    ) override;
};

class MainWindow : public BaseWindow<MainWindow> {
public:
    HWND hEdit, hButton, hScroll;
    HWND hListView;
public:
    MainWindow() {
        hEdit = nullptr;
        hButton = nullptr;
        hScroll = nullptr;
        hListView = nullptr;
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

    void CreateColumn();
    void AddItems(nlohmann::json context);
};

extern PanelWindow panel;
class CircleWindow {
public:
    static std::vector<HWND> nodes;
public:
    CircleWindow() { }

    static LRESULT CALLBACK NodeProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class LineWindow {
public:
    static LRESULT CALLBACK LineProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};