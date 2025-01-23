#include <windows.h>
#include "framework.h"
#include "WindowClass.h"

MainWindow win(L"Ai Asistance");
CircleWindow c1, c2;

using namespace Gdiplus;
Image* image;
HINSTANCE hInstance;

// program entry point
extern "C" __declspec(dllexport) int __stdcall ShowMyWindow() {

    if (!win.Create(L"Learn to Program Windows", WS_OVERLAPPEDWINDOW, hInstance))
    {
        return 0;
    }
    if (!c1.Create(nullptr, WS_CHILD | WS_VISIBLE, 0, 50, 50, 100, 100,
        win.m_hwnd, 0))
    {
        return 0;
    }
    if (!c2.Create(nullptr, WS_CHILD | WS_VISIBLE, 0, 200, 50, 100, 100,
        win.m_hwnd, 0))
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

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        hInstance = hModule;
    }
    return TRUE;
}