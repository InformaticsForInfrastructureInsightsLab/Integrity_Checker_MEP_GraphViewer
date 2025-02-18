#pragma once
#include <type_traits>
#include <string>

#include "framework.h"
#include "WindowClass.hpp"
#include "Graph.hpp"

// �ݹ� �Լ� Ÿ�� ����
using CallbackFunc = void(__stdcall*)(void);
using GUIDExportFunc = void(__stdcall*)(LPWSTR);

std::string LpwstrToString(LPWSTR str);

template<typename T>
typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>, void>
BuildGraph(T&& json_string);

extern "C" {
	// �ݹ� ��� �Լ�
	__declspec(dllexport) void __stdcall RegisterCallback(CallbackFunc callback);

	__declspec(dllexport) void __stdcall RegisterGUIDExportFunc(GUIDExportFunc guidExport);

	// export user question to extern process
	__declspec(dllexport) LPCWSTR ForwardQuestion();

	// get string from extern process
	__declspec(dllexport) void ForwardAnswer(LPWSTR result, LPWSTR context);
}

namespace GraphUtils {
	/// <summary>
	/// �� ���� �մ� ������ ���� �����ϴ� ������ ����ϴ� �Լ�
	/// ���� ������ ������ (x1,y1)���� (x2,y2)����
	/// ���� �߽��� (x1,y1)���� ����
	/// </summary>
	/// <param name="x1"> ���1 �߽��� x��ǥ </param>
	/// <param name="y1"> ���1 �߽��� y��ǥ </param>
	/// <param name="x2"> ���2 �߽���  x��ǥ </param>
	/// <param name="y2"> ���2 �߽���  y��ǥ </param>
	/// <param name="r"> ���� ������ </param>
	/// <param name="ix"> ������ x��ǥ�� ���� ������ ���۷��� </param>
	/// <param name="iy"> ������ y��ǥ�� ���� ������ ���۷��� </param>
	void GetIntersectionPoint(int x1, int y1, int x2, int y2, int r, int& ix, int& iy);
}