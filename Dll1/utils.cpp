#include "utils.hpp"

#include <memory>

extern MainWindow win;
extern PanelWindow panel;

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

	win.AddItems(json);

	//image = new Image(L"C:/objectinfo/gpt_visualize.png");
	RECT client_rect = { 0,0,950,340 };
	PostMessage(panel.m_hwnd, WM_UPDATE_GRAPH, NULL, reinterpret_cast<LPARAM>(graph));
}

// �ݹ� �Լ� ������ ���� ����
CallbackFunc g_callback = nullptr;
GUIDExportFunc g_guidExport = nullptr;
LPWSTR prevContext = nullptr;

extern "C" {
	// �ݹ� ��� �Լ�
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback) {
		SetWindowText(win.hAnswer, L" ");
		g_callback = callback;
	}

	__declspec(dllexport) void __stdcall RegisterGUIDExportFunc(GUIDExportFunc guidExport) {
		g_guidExport = guidExport;
	}

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion() {
		WCHAR text[4096];
		GetWindowText(win.hEdit, text, sizeof(text) / sizeof(WCHAR));
		return text;
	}

	__declspec(dllexport) LPCWSTR ForwardPrevContext() {
		return prevContext;
	}

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context) {
	 	// GPT�亯â�� �ؽ�Ʈ ����
		SetWindowText(win.hAnswer, result);

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