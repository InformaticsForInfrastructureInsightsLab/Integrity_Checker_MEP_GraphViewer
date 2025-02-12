#include "utils.hpp"

#include <memory>

extern MainWindow win;
extern PanelWindow panel;

using namespace Gdiplus;

std::string LpwstrToString(LPWSTR str) {
	// 변환에 필요한 버퍼 크기 계산
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	// 버퍼 할당
	std::string std_str(size_needed, 0);
	// 실제 변환 수행
	WideCharToMultiByte(CP_UTF8, 0, str, -1, &std_str[0], size_needed, NULL, NULL);

	return std_str;
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
}

// 콜백 함수 포인터 저장 변수
CallbackFunc g_callback = nullptr;

extern "C" {
	// 콜백 등록 함수
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback) {
		SetWindowText(win.hAnswer, L" ");
		g_callback = callback;
	}

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion() {
		WCHAR text[256];
		GetWindowText(win.hEdit, text, sizeof(text) / sizeof(WCHAR));
		return text;
	}

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context) {
		// GPT답변창의 텍스트 변경
		SetWindowText(win.hAnswer, result);

		try {
			std::string json = LpwstrToString(context);
			BuildGraph(json);
		}
		catch (const std::exception& e) {
			wchar_t wcStr[4096];
			size_t conv_chars;
			int len = (int)strlen(e.what()) + 1;
			errno_t err = mbstowcs_s(&conv_chars, wcStr, len, e.what(), _TRUNCATE);
			SetWindowText(win.hAnswer, wcStr);

			if (err != 0)
				return;
		}
	}
}

namespace GraphUtils {
	void GetIntersectionPoint(int x1, int y1, int x2, int y2, int r, int& ix, int& iy) {
		double dx = x2 - x1;
		double dy = y2 - y1;
		double len = sqrt(dx * dx + dy * dy);
		dx /= len;  // 방향 벡터의 단위화
		dy /= len;

		// 원의 중심에서 선을 따라 반지름만큼 이동하여 경계점 찾기
		ix = x1 + int(dx * r);
		iy = y1 + int(dy * r);
	}
}