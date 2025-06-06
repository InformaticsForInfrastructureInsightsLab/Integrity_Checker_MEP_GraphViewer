#pragma once
#include <type_traits>
#include <string>

#include "framework.h"
#include "WindowClass.hpp"
#include "Graph.hpp"

// 콜백 함수 타입 정의
using CallbackFunc = void(__stdcall*)(void);
using GUIDExportFunc = void(__stdcall*)(LPWSTR, LPWSTR);

std::string LpwstrToString(LPWSTR str);
LPWSTR StringToLpwstr(std::string str);

template<typename T>
typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, void>
BuildGraph(T&& json_string);

extern "C" {
	// 콜백 등록 함수
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback);

	__declspec(dllexport) void __stdcall RegisterGUIDExportFunc(GUIDExportFunc guidExport);

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion();

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context);

	__declspec(dllexport) void ForwardGraphKey(const char* key);
}
