#pragma once
#include "pch.h"
#include "handles.hpp"
#include "utils.hpp"

#include <string>

// 콜백 함수 타입 정의
using CallbackFunc = void(__stdcall*)(void);

// 콜백 함수 포인터 저장 변수
CallbackFunc g_callback = nullptr;

extern "C" {
	// 콜백 등록 함수
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
		// GPT답변창의 텍스트 변경
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
