#include "utils.hpp"

extern MainWindow win;

using namespace Gdiplus;
extern Image* image;

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
	if (image) {
		delete image;
	}

	nlohmann::json json = nlohmann::json::parse(json_string);
	Graph graph(json);
	graph.visualize();

	image = new Image(L"C:/objectinfo/gpt_visualize.png");
	RECT client_rect = { 0,0,900,500 };
	InvalidateRect(win.m_hwnd, &client_rect, TRUE);
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