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

extern std::vector<std::wstring> messages;

extern int totalHeight;
extern int scrollPos;

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

        hScroll = CreateWindowEx(0, L"SCROLLBAR", nullptr,
            WS_CHILD | WS_VISIBLE | SBS_VERT,
            920, 350, 20, 140, m_hwnd, (HMENU)1301, GetModuleHandle(NULL), nullptr);

        hListView = CreateWindowEx(
            WS_EX_CLIENTEDGE, WC_LISTVIEW, L" ",
            WS_CHILD | WS_VISIBLE | LVS_REPORT,
            645, 0, 300, 340, m_hwnd, (HMENU)3002, GetModuleHandle(NULL), NULL);
        CreateColumn();

        // 그래프 패널
        if (!panel.Create(L"GraphPanel",
            WS_CHILD | WS_VISIBLE | WS_BORDER | SS_NOTIFY, 0,
            10, 0, 640, 340, m_hwnd, (HMENU)3001)) {
            MessageBox(m_hwnd, L"panel create fail", L" ", MB_OK);
        }

        // 채팅 패널
        if (!chatPanel.Create(L"ChattingPanel",
            WS_CHILD | WS_VISIBLE | WS_BORDER | SBS_VERT | SS_NOTIFY, 0,
            10, 350, 920, 140, m_hwnd, (HMENU)3003)) {
            MessageBox(m_hwnd, L"chatting panel create fail", L" ", MB_OK);
        }
        break;    
    case WM_COMMAND:
        if (LOWORD(wParam) == 2001) {
            g_callback();
        }
        break;
    case WM_VSCROLL: {
        int scrollCode = LOWORD(wParam);
        int pos = HIWORD(wParam);

        switch (scrollCode) {
        case SB_LINEUP: scrollPos -= 10; break;
        case SB_LINEDOWN: scrollPos += 10; break;
        case SB_PAGEUP: scrollPos -= 50; break;
        case SB_PAGEDOWN: scrollPos += 50; break;
        case SB_THUMBTRACK: scrollPos = pos; break;
        }

        // 스크롤 위치 제한
        if (scrollPos < 0) scrollPos = 0;
        if (scrollPos > totalHeight - 400) scrollPos = totalHeight - 400;

        // 패널 다시 그리기
        InvalidateRect(chatPanel.m_hwnd, NULL, TRUE);
        SetScrollPos(hScroll, SB_CTL, scrollPos, TRUE);
        break;
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
        SendMessage(m_hwnd, WM_VSCROLL, (delta > 0 ? SB_LINEUP : SB_LINEDOWN), 0);
        break;
    }
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
    static float scale = 0.7f;
    static Graph* graph = nullptr;

    switch (uMsg) {
    case WM_CREATE: {
        // register node class
        WNDCLASS wc = {};
        wc.lpfnWndProc = CircleWindow::NodeProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = L"NODECLASS";
        RegisterClass(&wc);

        WNDCLASS wc_line = {};
        wc_line.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc_line.lpfnWndProc = LineWindow::LineProc;
        wc_line.hInstance = GetModuleHandle(NULL);
        wc_line.lpszClassName = L"LINECLASS";
        RegisterClass(&wc_line);
    } break;
    case WM_UPDATE_GRAPH: {
        if (graph) {
            graph->Release();
            delete graph;
        }
        graph = reinterpret_cast<Graph*>(lParam);
        RECT wnd_sz = { 0,0,width, height };
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

        if (graph)
            graph->RenderGraph(hMemDC, scale, offsetX, offsetY);

        BitBlt(hdc, 0, 0, width, height, hMemDC, 0, 0, SRCCOPY);

        // 5. 리소스 정리
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);

        EndPaint(m_hwnd, &ps);
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
    case WM_DESTROY:
        if (graph) {
            graph->Release();
            delete graph;
            graph = nullptr;
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

    RECT size = { x,y,nWidth,nHeight };
    AdjustWindowRect(&size, dwStyle, FALSE);

    width = size.right - size.left;
    height = size.bottom - size.top;

    m_hwnd = CreateWindowEx( 
        dwExStyle,
        ClassName(), lpWindowName, 
        dwStyle, 
        x, y, width, height,
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}
#pragma endregion

#pragma region ChatPanelWindow

LRESULT ChatPanelWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);

        RECT rc;
        GetClientRect(m_hwnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1)); // 기본 배경색

        int yOffset = -scrollPos + 10; // 스크롤 적용
        for (size_t i = 0; i < messages.size(); i++) {
            RECT bubbleRect = { 10, yOffset, 320, yOffset + 40 };

            // 말풍선 배경색 (연한 파란색)
            HBRUSH hBrush = CreateSolidBrush(RGB(173, 216, 230));
            FillRect(hdc, &bubbleRect, hBrush);
            DeleteObject(hBrush);

            // 말풍선 테두리 (검은색)
            HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            SelectObject(hdc, hPen);
            SelectObject(hdc, GetStockObject(NULL_BRUSH));

            RoundRect(hdc, bubbleRect.left, bubbleRect.top, bubbleRect.right, bubbleRect.bottom, 15, 15);

            DeleteObject(hPen);

            // 텍스트 출력
            SetBkMode(hdc, TRANSPARENT);
            DrawText(hdc, messages[i].c_str(), -1, &bubbleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            yOffset += 40 + 10; // 다음 메시지 위치
        }

        EndPaint(m_hwnd, &ps);
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

    RECT size = { x,y,nWidth,nHeight };
    AdjustWindowRect(&size, dwStyle, FALSE);

    width = size.right - size.left;
    height = size.bottom - size.top;

    m_hwnd = CreateWindowEx(
        dwExStyle,
        ClassName(), lpWindowName,
        dwStyle,
        x, y, width, height,
        hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}

#pragma endregion

#pragma region CircleWindow

LRESULT CALLBACK CircleWindow::NodeProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_CREATE: {
        RECT client;
        GetClientRect(hwnd, &client);
        int centX = (client.right + client.left) / 2;
        int centY = (client.bottom + client.top) / 2;
        int radX = (client.right - client.left) / 2;
        int radY = (client.bottom - client.top) / 2;
        HRGN hRgn = CreateEllipticRgn(centX - radX, centY - radY, centX + radX, centY + radY);
        SetWindowRgn(hwnd, hRgn, TRUE);
        DeleteObject(hRgn);
        break;
    }
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

        int fontSize = static_cast<int>(min(radX*0.5, radY) * 0.5);
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
        break;
    }
    case WM_LBUTTONDOWN: {
        Agnode_t* node = reinterpret_cast<Agnode_t*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        std::string guid_str = agget(node, const_cast<char*>("guid"));
        size_t wideLen = 0;
        mbstowcs_s(&wideLen, nullptr, 0, guid_str.c_str(), _TRUNCATE);
        LPWSTR guid = new WCHAR[wideLen];
        mbstowcs_s(&wideLen, guid, wideLen, guid_str.c_str(), _TRUNCATE);

        g_guidExport(guid, guid);
        break;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

#pragma endregion

#pragma region LineWindow

LRESULT CALLBACK LineWindow::LineProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg)
    {
    case WM_LBUTTONDOWN: {
        Agedge_t* edge = reinterpret_cast<Agedge_t*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        Agnode_t* start = aghead(edge);
        Agnode_t* end = agtail(edge);

        std::string start_guid_str = agget(start, const_cast<char*>("guid"));
        size_t wideLen = 0;
        mbstowcs_s(&wideLen, nullptr, 0, start_guid_str.c_str(), _TRUNCATE);
        LPWSTR start_guid = new WCHAR[wideLen];
        mbstowcs_s(&wideLen, start_guid, wideLen, start_guid_str.c_str(), _TRUNCATE);

        std::string end_guid_str = agget(end, const_cast<char*>("guid"));
        wideLen = 0;
        mbstowcs_s(&wideLen, nullptr, 0, end_guid_str.c_str(), _TRUNCATE);
        LPWSTR end_guid = new WCHAR[wideLen];
        mbstowcs_s(&wideLen, end_guid, wideLen, end_guid_str.c_str(), _TRUNCATE);

        g_guidExport(start_guid, end_guid);
        break;
    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

#pragma endregion