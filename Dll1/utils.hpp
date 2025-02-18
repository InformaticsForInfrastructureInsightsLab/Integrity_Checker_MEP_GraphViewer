#pragma once
#include <type_traits>
#include <string>

#include "framework.h"
#include "WindowClass.hpp"
#include "Graph.hpp"

// 콜백 함수 타입 정의
using CallbackFunc = void(__stdcall*)(void);
using GUIDExportFunc = void(__stdcall*)(LPWSTR);

std::string LpwstrToString(LPWSTR str);

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
}

namespace GraphUtils {
	/// <summary>
	/// 두 점을 잇는 직선이 원과 교차하는 지점을 계산하는 함수
	/// 직선 벡터의 방향은 (x1,y1)에서 (x2,y2)방향
	/// 원의 중심은 (x1,y1)으로 설정
	/// </summary>
	/// <param name="x1"> 노드1 중심의 x좌표 </param>
	/// <param name="y1"> 노드1 중심의 y좌표 </param>
	/// <param name="x2"> 노드2 중심의  x좌표 </param>
	/// <param name="y2"> 노드2 중심의  y좌표 </param>
	/// <param name="r"> 원의 반지름 </param>
	/// <param name="ix"> 교점의 x좌표를 받을 변수의 레퍼런스 </param>
	/// <param name="iy"> 교점의 y좌표를 받을 변수의 레퍼런스 </param>
	void GetIntersectionPoint(int x1, int y1, int x2, int y2, int r, int& ix, int& iy);
}