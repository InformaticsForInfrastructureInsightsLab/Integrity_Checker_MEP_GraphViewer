#include "utils.hpp"

#include <memory>

extern MainWindow win;
extern PanelWindow panel;

using namespace Gdiplus;

std::string LpwstrToString(LPWSTR str) {
	// ��ȯ�� �ʿ��� ���� ũ�� ���
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
	// ���� �Ҵ�
	std::string std_str(size_needed, 0);
	// ���� ��ȯ ����
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

// �ݹ� �Լ� ������ ���� ����
CallbackFunc g_callback = nullptr;

extern "C" {
	// �ݹ� ��� �Լ�
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
		// GPT�亯â�� �ؽ�Ʈ ����
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
		dx /= len;  // ���� ������ ����ȭ
		dy /= len;

		// ���� �߽ɿ��� ���� ���� ��������ŭ �̵��Ͽ� ����� ã��
		ix = x1 + int(dx * r);
		iy = y1 + int(dy * r);
	}
}