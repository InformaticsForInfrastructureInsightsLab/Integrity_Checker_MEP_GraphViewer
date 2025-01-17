#pragma once
#include "pch.h"
#include <type_traits>

#include "Graph.hpp"

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
typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>,void>
BuildGraph(T&& json_string) {
    nlohmann::json json = nlohmann::json::parse(json_string);
    Graph graph(json);
}