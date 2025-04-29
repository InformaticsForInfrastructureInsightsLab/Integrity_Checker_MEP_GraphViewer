#include "framework.h"
#include "WindowClass.hpp" 
#include <commctrl.h>
#include "Chat.h"

#pragma comment(lib, "comctl32.lib")

#include "utils.hpp"
extern CallbackFunc g_callback;
extern GUIDExportFunc g_guidExport;

extern MainWindow win;
extern PanelWindow panel;
extern ChatPanelWindow chatPanel;

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

    RECT size = { 0, 0, nWidth, nHeight };
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
        hListView = CreateWindowEx(
            0, WC_LISTVIEW, L" ",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,
            645, 0, 300, 340, m_hwnd, (HMENU)3002, GetModuleHandle(NULL), NULL);
        CreateColumn();

        // 그래프 패널
        if (!panel.Create(L"GraphPanel",
            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER, 0,
            10, 0, 640, 340, m_hwnd, (HMENU)3001)) {
            MessageBox(m_hwnd, L"panel create fail", L" ", MB_OK);
        }

        // 채팅 패널
        if (!chatPanel.Create(L"ChattingPanel",
            WS_CHILD | WS_VISIBLE | WS_BORDER, 0,
            10, 350, 930, 140, m_hwnd, (HMENU)3003)) {
            MessageBox(m_hwnd, L"chatting panel create fail", L" ", MB_OK);
        }
        break;  
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);
        break;
    }        
    case WM_COMMAND:
        if (LOWORD(wParam) == 2001) {
            g_callback();
        }
        break;
    case WM_CLOSE:
        DestroyWindow(m_hwnd);
        break;
    case WM_DESTROY:
        UnregisterClass(ClassName(), GetModuleHandle(NULL));
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void MainWindow::CreateColumn() {
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.pszText = const_cast<LPWSTR>(L"GUID1");
    lvc.cx = 50;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 0, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"GUID2");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 1, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"Clash Type");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 2, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"topology(x+y+z)");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 3, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"hard clash type");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 4, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"soft clash type");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 5, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"adjusted severity");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 6, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"severity");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 7, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"Clearance");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 8, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"offset");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 9, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"penetrability");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 10, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"ABS_Volume_Diff");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 11, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"ABS_Volume_Sum");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 12, &lvc);

    lvc.pszText = const_cast<LPWSTR>(L"Clash Volume");
    lvc.cx = 150;
    lvc.iSubItem = 0;
    ListView_InsertColumn(hListView, 12, &lvc);
}

void MainWindow::AddItems(nlohmann::json context) {
    for (auto clash : context) {
        LVITEM lvi;
        lvi.mask = LVIF_TEXT;
        lvi.iItem = 0;
        lvi.iSubItem = 0;
        lvi.pszText = StringToLpwstr(clash["m"]["properties"]["GUID"]);
        ListView_InsertItem(hListView, &lvi);

        lvi.iSubItem = 1;
        lvi.pszText = StringToLpwstr(clash["n"]["properties"]["GUID"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 2;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["HardClash"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 3;
        std::string topology = std::string(clash["r"]["properties"]["TopologyXaxis"]) + ","
            + std::string(clash["r"]["properties"]["TopologyYaxis"]) + ","
            + std::string(clash["r"]["properties"]["TopologyZaxis"]);
        lvi.pszText = StringToLpwstr(topology);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 4;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["ClashType"]);
        ListView_SetItem(hListView, &lvi);

        if (clash["r"]["properties"]["HardClash"] == "False") {
            lvi.iSubItem = 5;
            lvi.pszText = StringToLpwstr(clash["r"]["properties"]["SoftClash"]);
            ListView_SetItem(hListView, &lvi);
        }

        lvi.iSubItem = 6;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["Severity_Prediction"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 7;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["Severity"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 8;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["Clearance"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 9;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["Offset"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 10;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["Penetration"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 11;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["ABS_Volume_Diff"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 12;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["ABS_Volume_Sum"]);
        ListView_SetItem(hListView, &lvi);

        lvi.iSubItem = 13;
        lvi.pszText = StringToLpwstr(clash["r"]["properties"]["Clash_Volume"]);
        ListView_SetItem(hListView, &lvi);
    }
}

#pragma endregion


#pragma region PanelWindow
LRESULT PanelWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static float scale = 1.0f;
    static Graph* graph = nullptr;
    static bool isDragging = false;
    static POINT lastMousePos = { 0,0 };

    static bool isNewGraph = true;
    static int dx, dy;

    static HFONT arial = nullptr;

	switch (uMsg) {
	case WM_CREATE: {
		arial = CreateFont(
            -16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            L"Arial"
        );
        break;
	}
    case WM_UPDATE_GRAPH: {
        if (graph) {
            graph->Release();
            delete graph;
        }
        graph = reinterpret_cast<Graph*>(lParam);
        RECT wnd_sz = { 0,0,width, height };
        offsetX = 0; 
        offsetY = 0;
        scale = 1.0f;
        isNewGraph = true;
        InvalidateRect(m_hwnd, &wnd_sz, true);        
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        // 1. 백버퍼 생성 (메모리 DC)
        HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

        // 배경을 흰색으로 지움
        PatBlt(hMemDC, 0, 0, width, height, WHITENESS);

        //if (isNewGraph) {
        if (!graph) break;

        graph->RenderGraph(hMemDC, scale, offsetX, offsetY);

        for (auto edge : graph->visitedEdges) {
            HPEN hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
            HPEN hOldPen = (HPEN)SelectObject(hMemDC, hPen);

            MoveToEx(hMemDC, edge->start_logicX, edge->start_logicY, NULL);
            LineTo(hMemDC, edge->end_logicX, edge->end_logicY);

            SelectObject(hMemDC, hOldPen);
            DeleteObject(hPen);
        }

        for (auto node : graph->visitedNodes) {
            HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
            HBRUSH hBrush = CreateSolidBrush(RGB(135, 206, 235));

            HPEN hOldPen = (HPEN)SelectObject(hMemDC, hPen);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

            Ellipse(hMemDC,
                node->logicX - node->logicRad, node->logicY - node->logicRad,
                node->logicX + node->logicRad, node->logicY + node->logicRad
            );

            std::string type = agget(node->node, const_cast<char*>("element_type"));
            RECT textRect;
            textRect.left = node->logicX - node->logicRad;
            textRect.top = node->logicY - node->logicRad;
            textRect.right = node->logicX + node->logicRad;
            textRect.bottom = node->logicY + node->logicRad;

            HFONT hOldFont = (HFONT)SelectObject(hMemDC, arial);

            SetBkMode(hMemDC, TRANSPARENT);
            DrawTextA(hMemDC, type.c_str(), -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hMemDC, hOldPen);
            SelectObject(hMemDC, hOldBrush);
            SelectObject(hMemDC, hOldFont);
            DeleteObject(hBrush);
        }

        BitBlt(hdc, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);

        // 5. 리소스 정리
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);

        EndPaint(m_hwnd, &ps);
        break;
    }
    case WM_ERASEBKGND: {
        RECT rect;
        GetClientRect(m_hwnd, &rect);
        FillRect((HDC)wParam, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        break;
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);  // 휠 방향 (120 또는 -120)
        POINT cursor;
        GetCursorPos(&cursor);
        ScreenToClient(m_hwnd, &cursor);  // 윈도우 기준 좌표로 변환

        double oldScale = scale;
        scale += (delta > 0) ? 0.1 : -0.1;
        if (scale < 0.1) scale = 0.1;

        // 마우스 좌표 중심으로 확대/축소 변환
        offsetX = cursor.x - (cursor.x - offsetX) * (scale / oldScale);
        offsetY = cursor.y - (cursor.y - offsetY) * (scale / oldScale);

        // 커스텀 이벤트로 원 다시 그리기 요청
        RECT wnd_sz = { 0,0,width, height };
        InvalidateRect(m_hwnd, &wnd_sz, true);
        break;
    }
    case WM_LBUTTONDOWN: {
        SetCapture(m_hwnd);        
        lastMousePos.x = GET_X_LPARAM(lParam);
        lastMousePos.y = GET_Y_LPARAM(lParam);
        
        Agedge_t* edge = HitTestEdge(lastMousePos.x, lastMousePos.y, graph);
        if (edge) {
            MessageBox(m_hwnd, L" ", L" ", MB_OK);
            std::string start = agget(edge, const_cast<char*>("start_node"));
            std::string end = agget(edge, const_cast<char*>("end_node"));
            g_guidExport(StringToLpwstr(start), StringToLpwstr(end));
            ReleaseCapture();
            return 1;
        }

        Agnode_t* node = HitTestNode(lastMousePos.x, lastMousePos.y, graph);
        if (node) {
            std::string type = agget(node, const_cast<char*>("element_type"));
            LPWSTR LPtype = StringToLpwstr(type);
            g_guidExport(LPtype, LPtype);
            ReleaseCapture();
            return 1;
        }        

        isDragging = true;
        break;
    }
    case WM_MOUSEMOVE:
        if (isDragging) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            dx = x - lastMousePos.x;
            dy = y - lastMousePos.y;

            offsetX += dx; 
            offsetY += dy;

            lastMousePos.x = x;
            lastMousePos.y = y;

            InvalidateRect(m_hwnd, NULL, true);
        }
        break;
    case WM_LBUTTONUP:
        ReleaseCapture();
        isDragging = false;
        break;
    case WM_DESTROY:
        if (graph) {
            graph->Release();
            delete graph;
            graph = nullptr;
        }
        if (arial) {
            DeleteObject(arial);
            arial = nullptr;
        }
        UnregisterClass(ClassName(), GetModuleHandle(NULL));
        break;
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
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
    RegisterClass(&wc);

    width = nWidth;
    height = nHeight;

    m_hwnd = CreateWindowEx( 
        dwExStyle,
        ClassName(), lpWindowName, 
        dwStyle, 
        x, y, nWidth, nHeight,
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}
#pragma endregion

#pragma region ChatPanelWindow

LRESULT ChatPanelWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int scrollOffset = 0;
    static int totalContentHeight = 0;

    switch (uMsg) {
    case WM_UPDATE_CHAT: {
        ChatMessage* Pmsg = reinterpret_cast<ChatMessage*>(lParam);
        messages.push_back(*Pmsg);
        delete Pmsg;

        HDC hdc = GetDC(m_hwnd);
        RECT r;
        int newMsgHeight = AddMessageAndMeasure(hdc, messages.back(), box_max_width, r, totalContentHeight);
        ReleaseDC(m_hwnd, hdc);

        // 기존 내용 위로 스크롤
        ScrollWindowEx(m_hwnd, 0, -newMsgHeight, NULL, NULL, NULL, NULL, SW_INVALIDATE);
        totalContentHeight += newMsgHeight + 10;

        scrollOffset = max(0, totalContentHeight - height);
        SetScrollPos(m_hwnd, SB_VERT, scrollOffset, TRUE);
        InvalidateRect(m_hwnd, NULL, TRUE);

        // 스크롤 정보 갱신
        SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS };
        si.nMin = 0;
        si.nMax = totalContentHeight;
        si.nPage = height;
        si.nPos = scrollOffset;
        SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);

        // 하단 새 메시지 영역만 다시 그림
        RECT invalidRect = { 0, height - newMsgHeight, width, height };
        InvalidateRect(m_hwnd, &invalidRect, TRUE);
        break;
    }    
    case WM_SIZE:
        height = HIWORD(lParam);
        {
            SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE | SIF_POS };
            si.nMin = 0;
            si.nMax = totalContentHeight;
            si.nPage = height;
            si.nPos = scrollOffset;
            SetScrollInfo(m_hwnd, SB_VERT, &si, TRUE);
        }
        break;
    case WM_VSCROLL: {
        int pos = scrollOffset;

        switch (LOWORD(wParam)) {
        case SB_LINEUP: pos -= 20; break;
        case SB_LINEDOWN: pos += 20; break;
        case SB_PAGEUP: pos -= 20; break;
        case SB_PAGEDOWN: pos += 20; break;
        case SB_THUMBTRACK: pos = HIWORD(wParam); break;
        }

        pos = max(0, min(pos, totalContentHeight - height));
        if (pos != scrollOffset) {
            scrollOffset = pos;
            SetScrollPos(m_hwnd, SB_VERT, scrollOffset, TRUE);
            InvalidateRect(m_hwnd, NULL, TRUE);
        }
        break;
    }
    case WM_ERASEBKGND: {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1)); // 배경색 흰색
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        int y = 10 - scrollOffset;

        for (const auto& msg : messages) {
            RECT r;
            int h = AddMessageAndMeasure(hdc, msg, box_max_width, r, y);
            DrawBalloon(hdc, r, msg);
            y += h + 10;
        }

        EndPaint(m_hwnd, &ps);
        break;
    }    
    case WM_DESTROY: {
        scrollOffset = 0;
        totalContentHeight = 0;
        messages.clear();
        break;
    }
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL ChatPanelWindow::Create(PCWSTR lpWindowName,
    DWORD dwStyle, DWORD dwExStyle,
    int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu) {

    WNDCLASS wc = { 0 };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = ClassName();
    RegisterClass(&wc);

    width = nWidth;
    height = nHeight;

    m_hwnd = CreateWindowEx(
        dwExStyle,
        ClassName(), lpWindowName,
        dwStyle,
        x, y, nWidth, nHeight,
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}

#pragma endregion
