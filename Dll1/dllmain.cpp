// MyWindowDLL.cpp
#include <windows.h>
#include <string>
#include <vector>

#include "pch.h"
#include "callback.hpp"

HINSTANCE hInstance;
HWND hWnd, hEdit, hButton, hAnswer;

// 콜백 함수 타입 정의
using CallbackFunc = void(__stdcall*)(const char*);

// 콜백 함수 포인터 저장 변수
CallbackFunc g_callback = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern "C" {
    // 콜백 등록 함수
    __declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback) {
        g_callback = callback;
    }

    // C++에서 콜백 호출 (이벤트 발생 시)
    __declspec(dllexport) void __stdcall TriggerEvent(LPWSTR text) {
        // 변환에 필요한 버퍼 크기 계산
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, 0, NULL, NULL);
        // 버퍼 할당
        std::string strTo(size_needed, 0);
        // 실제 변환 수행
        WideCharToMultiByte(CP_UTF8, 0, text, -1, &strTo[0], size_needed, NULL, NULL);
        g_callback(strTo.c_str());
    }

    __declspec(dllexport) void ForwardAnswer(LPWSTR answer) {
        // 버튼의 텍스트 변경
        SetWindowText(hAnswer, answer);
    }

    __declspec(dllexport) void __stdcall ShowMyWindow() {
        const wchar_t CLASS_NAME[] = L"MyWindowClass";

        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        RECT size = { 0, 0, 950, 600 };
        DWORD dwStyle = WS_OVERLAPPEDWINDOW;
        AdjustWindowRect(&size, dwStyle, FALSE);
        hWnd = CreateWindow(
            wc.lpszClassName,         // 윈도우 클래스 이름
            TEXT("AI Chat"),  // 창 제목
            dwStyle,                  // 창 스타일
            CW_USEDEFAULT, CW_USEDEFAULT,
            size.right - size.left,   // 전체 창 너비
            size.bottom - size.top,   // 전체 창 높이
            NULL, NULL, hInstance, NULL
        );

        if (hWnd) {
            ShowWindow(hWnd, SW_SHOW);
            UpdateWindow(hWnd);

            MSG msg = {};
            while (GetMessage(&msg, nullptr, 0, 0)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        hInstance = hModule;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        // 입력 칸 (Edit Control) 생성
        hEdit = CreateWindowEx(
            0, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            10, 500, 830, 90,
            hwnd, (HMENU)1001,
            hInstance, nullptr
        );

        // 버튼 생성
        hButton = CreateWindowEx(
            0, L"BUTTON", L"SEND",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            850, 500, 90, 90,
            hwnd, (HMENU)0001, hInstance, nullptr
        );

        //모델 답변
        hAnswer = CreateWindowEx(
            WS_EX_CLIENTEDGE,          
            L"EDIT",                    
            L"LLM의 답변이 여기에 표시됩니다",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            10, 350, 930, 140,        
            hwnd, (HMENU)1002,                
            hInstance, nullptr);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            WCHAR text[256];
            GetWindowText(hEdit, text, sizeof(text)/sizeof(WCHAR));
            TriggerEvent(text);
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}