#pragma once
#include <type_traits>
#include <string>

#include "framework.h"
#include "WindowClass.hpp"
#include "Graph.hpp"

// �ݹ� �Լ� Ÿ�� ����
using CallbackFunc = void(__stdcall*)(void);

std::string LpwstrToString(LPWSTR str);

template<typename T>
typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, void>
BuildGraph(T&& json_string);

extern "C" {
	// �ݹ� ��� �Լ�
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback);

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion();

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context);
}