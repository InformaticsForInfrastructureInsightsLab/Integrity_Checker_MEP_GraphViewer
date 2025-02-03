#include <windows.h>
#include "framework.h"
#include "WindowClass.hpp"
#include "utils.hpp"

MainWindow win;
CircleWindow c1, c2;
PanelWindow panel;

using namespace Gdiplus;
Image* image;
HINSTANCE hInstance;

// program entry point
extern "C" __declspec(dllexport) int __stdcall ShowMyWindow() {
    // GDI+ 초기화
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    if (!win.Create(L"Ai Assistance", WS_OVERLAPPEDWINDOW))
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

    delete image;
    image = nullptr;
    GdiplusShutdown(gdiplusToken);

    return static_cast<int>(msg.wParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        hInstance = hModule;
    }
    return TRUE;
}