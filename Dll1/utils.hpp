#pragma once
#include "pch.h"

std::string&& LpwstrToString(LPWSTR str) {
    // ��ȯ�� �ʿ��� ���� ũ�� ���
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    // ���� �Ҵ�
    std::string std_str(size_needed, 0);
    // ���� ��ȯ ����
    WideCharToMultiByte(CP_UTF8, 0, str, -1, &std_str[0], size_needed, NULL, NULL);

    return std::move(std_str);
}