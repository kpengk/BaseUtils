#pragma once
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#if __cplusplus > 201703L
#include <ranges>
#endif

template <typename StringT = std::string_view>
std::vector<StringT> split_string(std::string_view str, std::string_view delimiter, bool keep_empty = true) {
    std::vector<StringT> tokens;
    std::size_t start = 0;
    std::size_t end = 0;

    while ((end = str.find(delimiter, start)) != std::string::npos) {
        if (end != start || keep_empty) {
            tokens.emplace_back(str.data() + start, end - start);
        }
        start = end + delimiter.length();
    }

    // Handle last token
    if (start < str.size() || (keep_empty && !str.empty())) {
        tokens.emplace_back(str.data() + start, str.size() - start);
    }

    return tokens;
}

template <typename StringT = std::string_view>
std::vector<StringT> split_string(std::string_view str, char delimiter, bool keep_empty = false) {
    return split_string(str, std::string_view(&delimiter, 1), keep_empty);
}

#if __cplusplus > 201703L
inline auto split_view(std::string_view str, std::string_view delim) {
    return str | std::views::split(delim)
         | std::views::transform([](auto&& rng) { return std::string_view(rng.begin(), rng.end()); });
}

inline std::vector<std::string_view> split_to_vector(std::string_view str, std::string_view delim) {
    auto view = split_view(str, delim);
    return {view.begin(), view.end()};
}
#endif
