#include "utils.hpp"
#include "Chat.h"

#include <memory>

extern MainWindow win;
extern PanelWindow panel;
extern ChatPanelWindow chatPanel;

std::string LpwstrToString(LPWSTR str) {
	// 변환에 필요한 버퍼 크기 계산
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	// 버퍼 할당
	std::string std_str(size_needed, 0);
	// 실제 변환 수행
	WideCharToMultiByte(CP_UTF8, 0, str, -1, &std_str[0], size_needed, NULL, NULL);

	return std_str;
}

LPWSTR StringToLpwstr(std::string str) {
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	LPWSTR lpwstr = new wchar_t[size_needed];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, lpwstr, size_needed);
	return lpwstr;
}

template<typename T>
typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, void>
BuildGraph(T&& json_string) {

	nlohmann::json json = nlohmann::json::parse(json_string);
	Graph* graph = new Graph(json);
	graph->buildGraph();
	graph->exportGraphImage();

	//image = new Image(L"C:/objectinfo/gpt_visualize.png");
	RECT client_rect = { 0,0,950,340 };
	PostMessage(panel.m_hwnd, WM_UPDATE_GRAPH, NULL, reinterpret_cast<LPARAM>(graph));

	win.AddItems(json);
}

// 콜백 함수 포인터 저장 변수
CallbackFunc g_callback = nullptr;
GUIDExportFunc g_guidExport = nullptr;
LPWSTR prevContext = nullptr;

extern int totalHeight;
extern int scrollPos;

// 채팅 메시지를 패널에 추가
void AddChatMessage(const std::wstring& message) {
	auto* copied_msg = new std::wstring(message);
	PostMessage(chatPanel.m_hwnd, WM_UPDATE_CHAT, NULL, reinterpret_cast<LPARAM>(copied_msg));
}

extern "C" {
	// 콜백 등록 함수
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback) {
		g_callback = callback;
	}

	__declspec(dllexport) void __stdcall RegisterGUIDExportFunc(GUIDExportFunc guidExport) {
		g_guidExport = guidExport;
	}

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion() {
		WCHAR text[4096];
		GetWindowText(win.hEdit, text, sizeof(text) / sizeof(WCHAR));

		AddChatMessage(text);

		SetWindowText(win.hEdit, L" ");
		return text;
	}

	__declspec(dllexport) LPCWSTR ForwardPrevContext() {
		return prevContext;
	}

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context) {
		AddChatMessage(result);
		try {
			prevContext = context;
			std::string json = LpwstrToString(context);
			BuildGraph(json);
		}
		catch (const std::exception& e) {
			wchar_t wcStr[4096];
			size_t conv_chars;
			int len = (int)strlen(e.what()) + 1;
			errno_t err = mbstowcs_s(&conv_chars, wcStr, len, e.what(), _TRUNCATE);
			MessageBox(win.m_hwnd, wcStr, L"ERROR", MB_OK);

			if (err != 0)
				return;
		}
	}
}