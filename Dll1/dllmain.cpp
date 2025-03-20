#include <windows.h>
#include "framework.h"
#include "WindowClass.hpp"
#include "utils.hpp"

MainWindow win;
PanelWindow panel;
ChatPanelWindow chatPanel;
HINSTANCE hInstance;

// program entry point
extern "C" __declspec(dllexport) int __stdcall ShowMyWindow() {

    if (!win.Create(L"Ai Assistance", WS_OVERLAPPEDWINDOW, 0,
        CW_USEDEFAULT, CW_USEDEFAULT, 950, 600))
    {
        return 0;
    }

    ShowWindow(win.m_hwnd, SW_SHOW);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        hInstance = hModule;
    }
    return TRUE;
}