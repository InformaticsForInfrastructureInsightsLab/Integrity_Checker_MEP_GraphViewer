#pragma once
#include "pch.h"

std::string&& LpwstrToString(LPWSTR str) {
    // 변환에 필요한 버퍼 크기 계산
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    // 버퍼 할당
    std::string std_str(size_needed, 0);
    // 실제 변환 수행
    WideCharToMultiByte(CP_UTF8, 0, str, -1, &std_str[0], size_needed, NULL, NULL);

    return std::move(std_str);
}