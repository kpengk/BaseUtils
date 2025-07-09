#pragma once
#if __has_include(<version>)
#include <version>
#endif
#include <map>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <format>
#include <array>
#ifdef __cpp_lib_concepts
#include <concepts>
#endif

namespace val2str {
    namespace detail {
        inline std::string map_to_string(const auto& values) {
            std::string str;
            str.reserve(1024);
            for (const auto& [key, val] : values) {
                str.append(std::format("{}:{},", key, val));
            }
            return str.substr(0, str.size() - 1);
        }

        template <typename InIt, typename UnaryOp>
        std::string to_string(InIt first, InIt last, UnaryOp unary_op) {
            if (first == last) {
                return "[]";
            }

            std::stringstream ss;
            ss << "[";
            for (; first != std::prev(last); ++first) {
                ss << unary_op(*first) << ",";
            }
            ss << unary_op(*std::prev(last)) << "]";
            return ss.str();
        }

        template <typename InIt>
        std::string to_string(InIt first, InIt last) {
            if (first == last) {
                return "[]";
            }

            std::stringstream ss;
            ss << "[";
            std::copy(first, std::prev(last), std::ostream_iterator<typename InIt::value_type>(ss, ","));
            ss << *std::prev(last) << "]";
            return ss.str();
        }
    }

    template <typename T, typename UnaryOp>
    std::string to_string(std::span<const T> values, UnaryOp unary_op) {
        return detail::to_string(values.begin(), values.end(), unary_op);
    }

    template <typename T, typename UnaryOp>
    std::string to_string(const std::vector<T>& values, UnaryOp unary_op) {
        return detail::to_string(values.begin(), values.end(), unary_op);
    }

    template <typename T, std::size_t SIZE, typename UnaryOp>
    std::string to_string(const std::array<T, SIZE>& values, UnaryOp unary_op) {
        return detail::to_string(values.begin(), values.end(), unary_op);
    }

    template <typename T, typename UnaryOp>
    std::string to_string(const std::set<T>& values, UnaryOp unary_op) {
        return detail::to_string(values.begin(), values.end(), unary_op);
    }

#ifdef __cpp_lib_concepts
    template <typename T>
        requires std::integral<T> || std::floating_point<T>
#else
    template <typename T, typename std::enable_if_t<std::is_integral_v<T> | std::is_floating_point_v<T>>* = nullptr>
#endif
    std::string to_string(std::span<const T> values) {
        return detail::to_string(values.begin(), values.end());
    }

    template <typename T>
    std::string to_string(const std::vector<T>& values) {
        return detail::to_string(values.begin(), values.end());
    }

    template <typename T, std::size_t SIZE>
    std::string to_string(const std::array<T, SIZE>& values) {
        return detail::to_string(values.begin(), values.end());
    }

    template <typename T>
    std::string to_string(const std::set<T>& values) {
        return detail::to_string(values.begin(), values.end());
    }

    // map、unordered_map
#ifdef __cpp_lib_concepts
    template <typename T1, typename T2>
        requires(std::integral<T1> || std::floating_point<T1> || std::is_same_v<std::remove_cvref_t<T1>, std::string>)
                && (std::integral<T2> || std::floating_point<T2> || std::is_same_v<std::remove_cvref_t<T2>, std::string>)
#else
    template <typename T1, typename T2,
        typename std::enable_if_t<
            (std::is_integral_v<T1> | std::is_floating_point_v<T1> | std::is_same_v<std::remove_cvref_t<T1>, std::string>)
        && (std::is_integral_v<T2> | std::is_floating_point_v<T2> | std::is_same_v<std::remove_cvref_t<T2>, std::string>)>* = nullptr>
#endif
    std::string to_string(const std::map<T1, T2>& values) {
        return detail::map_to_string(values);
    }

#ifdef __cpp_lib_concepts
    template <typename T1, typename T2>
        requires(std::integral<T1> || std::floating_point<T1> || std::is_same_v<std::remove_cvref_t<T1>, std::string>)
                && (std::integral<T2> || std::floating_point<T2> || std::is_same_v<std::remove_cvref_t<T2>, std::string>)
#else
    template <typename T1, typename T2,
        typename std::enable_if_t<
            (std::is_integral_v<T1> | std::is_floating_point_v<T1> | std::is_same_v<std::remove_cvref_t<T1>, std::string>)
        && (std::is_integral_v<T2> | std::is_floating_point_v<T2> | std::is_same_v<std::remove_cvref_t<T2>, std::string>)>* = nullptr>
#endif
    std::string to_string(const std::unordered_map<T1, T2>& values) {
        return detail::map_to_string(values);
    }
} // namespace val2str
