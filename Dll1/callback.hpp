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
		if (context != L"") {
			// GPT�亯â�� �ؽ�Ʈ ����
			SetWindowText(hAnswer, result);

			std::string graph = LpwstrToString(context);
		}			
	}
}
