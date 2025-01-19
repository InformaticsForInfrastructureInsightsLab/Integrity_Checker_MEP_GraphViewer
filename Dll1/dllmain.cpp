// MyWindowDLL.cpp
#include "pch.h"
#include "callback.hpp"
#include "handles.hpp"

constexpr int clientWidth = 950;
constexpr int clientHeight = 600;

using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// program entry point
extern "C" __declspec(dllexport) int __stdcall ShowMyWindow() {
	// GDI+ 초기화
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	const wchar_t CLASS_NAME[] = L"MyWindowClass";

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	RECT size = { 0, 0, clientWidth, clientHeight };
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
	
	MSG msg = {};
	if (hWnd) {
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);

		
		while (GetMessage(&msg, nullptr, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete image;
	GdiplusShutdown(gdiplusToken);

	return static_cast<int>(msg.wParam);
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
			g_callback();
		}
		break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// 배경을 흰색으로 채우기
		HBRUSH whiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
		FillRect(hdc, &ps.rcPaint, whiteBrush);

		// GDI+ Graphics 객체 생성
		Graphics graphics(hdc);

		// 이미지 그리기
		if (image)
		{
			RECT clientRect;
			GetClientRect(hwnd, &clientRect);

			int img_region_width = 950;
			int img_region_height = 350;

			// 이미지 크기 가져오기
			int imageWidth = image->GetWidth();
			int imageHeight = image->GetHeight();

			// 중앙 좌표 계산
			int x = (img_region_width - imageWidth) / 2;
			int y = (img_region_height - imageHeight) / 2;

			// 이미지 그리기
			graphics.DrawImage(image, x, y, imageWidth, imageHeight);
		}

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		UnregisterClass(L"MyWindowClass", hInstance);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}