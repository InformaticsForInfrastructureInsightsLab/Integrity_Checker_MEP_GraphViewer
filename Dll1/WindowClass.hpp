#pragma once
#include <windows.h>
#include "Graph.hpp"
#include "Chat.h"

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

class ChatPanelWindow : public BaseWindow<ChatPanelWindow> {
    int width, height;
    std::vector<ChatMessage> messages;
    int box_max_width;
public:
    ChatPanelWindow() : width(0), height(0), box_max_width(400) { }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    PCWSTR ClassName() const { return L"ChatWindowPanel"; }

    BOOL Create(PCWSTR lpWindowName,
        DWORD dwStyle, DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0
    );

private:
    int AddMessageAndMeasure(HDC hdc, const ChatMessage& msg, int maxWidth, RECT& outRect, int yStart) {
        outRect = { 10, yStart, 20 + maxWidth, yStart };
        DrawText(hdc, msg.Text().c_str(), -1, &outRect, DT_WORDBREAK | DT_CALCRECT);
        if (msg.IsMyChat()) {
            long box_width = outRect.right - outRect.left;
            outRect.right = width - 10 - 20; // 10은 패딩, 20은 스크롤바 고려
            outRect.left = outRect.right - box_width;
        }

        return outRect.bottom - yStart + 20;
    }

    void DrawBalloon(HDC hdc, const RECT& r, const ChatMessage& text) {
        int radius = 12;

        HBRUSH hBrush = CreateSolidBrush(text.IsMyChat() ?  RGB(135, 206, 235) : RGB(211,211,211));
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));

        RoundRect(hdc, r.left - 10, r.top - 5, r.right + 10, r.bottom + 5, radius, radius);

        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
        SelectObject(hdc, hOldPen);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawText(hdc, text.Text().c_str(), -1, const_cast<RECT*>(&r), DT_WORDBREAK | DT_LEFT | DT_TOP);
    }

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