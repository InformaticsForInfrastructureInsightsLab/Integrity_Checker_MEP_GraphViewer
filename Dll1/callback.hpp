#pragma once
#include "pch.h"
#include "handles.hpp"
#include "utils.hpp"

#include <string>

// �ݹ� �Լ� Ÿ�� ����
using CallbackFunc = void(__stdcall*)(void);

// �ݹ� �Լ� ������ ���� ����
CallbackFunc g_callback = nullptr;

extern "C" {
	// �ݹ� ��� �Լ�
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback) {
		SetWindowText(hAnswer, L" ");
		g_callback = callback;
	}

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion() {
		WCHAR text[256];
		GetWindowText(hEdit, text, sizeof(text) / sizeof(WCHAR));
		return text;
	}

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context) {
		// GPT�亯â�� �ؽ�Ʈ ����
		SetWindowText(hAnswer, result);

		try {
			std::string json = LpwstrToString(context);
			BuildGraph(json);
		}
		catch (const std::exception& e){
			wchar_t wcStr[4096];
			size_t conv_chars;
			int len = (int)strlen(e.what()) + 1;
			errno_t err = mbstowcs_s(&conv_chars, wcStr, len, e.what(), _TRUNCATE);
			SetWindowText(hAnswer, wcStr);

			if (err != 0)
				return;
		}
	}
}
