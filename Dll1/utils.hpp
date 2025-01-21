#pragma once
#include "pch.h"
#include <type_traits>

#include "Graph.hpp"

std::string LpwstrToString(LPWSTR str) {
    // 변환에 필요한 버퍼 크기 계산
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    // 버퍼 할당
    std::string std_str(size_needed, 0);
    // 실제 변환 수행
    WideCharToMultiByte(CP_UTF8, 0, str, -1, &std_str[0], size_needed, NULL, NULL);

    return std_str;
}

template<typename T>
typename std::enable_if_t<std::is_same_v<std::decay_t<T>, std::string>,void>
BuildGraph(T&& json_string) {
    nlohmann::json json = nlohmann::json::parse(json_string);
    Graph graph(json);
    graph.visualize();

    if (image) {
        delete image;
    }

    image = new Image(L"C:/objectinfo/gpt_visualize.png");
    RECT client_rect = { 0,0,900,500 };
    InvalidateRect(hWnd, &client_rect, TRUE);
}