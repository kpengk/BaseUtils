#pragma once

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace detail {
    template<typename Container>
    Container read_binary_file(const std::string &filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            spdlog::error("Open file failed: {}.", filename);
            return Container{};
        }
        const std::size_t file_len = std::filesystem::file_size(filename);
        Container content(file_len, 0x00);
        file.read(reinterpret_cast<char*>(content.data()), static_cast<std::streamsize>(file_len));
        return content;
    }
}

/*!
 * @brief  创建本地路径
 * @param  dir_path 需要创建的路径，如 out/data
 * @return 创建的绝对路径，如 x:/xxx/out/data/
 */
inline std::string create_path(std::string_view dir_path) {
    if (dir_path.empty()) {
        return std::string{};
    }

    const auto &syspath{std::filesystem::path(std::string(dir_path))};
    if (!std::filesystem::exists(syspath)) {
        try {
            std::filesystem::create_directories(syspath);
        } catch (const std::exception &ex) {
            spdlog::error("Failed create path: {}, exception:{}.", syspath.string(), ex.what());
            return std::string{};
        }
    }

    std::string str_path = std::filesystem::absolute(syspath).string();
    std::replace(str_path.begin(), str_path.end(), '\\', '/');
    if (str_path.back() != '/') {
        str_path.push_back('/');
    }
    return str_path;
}

/*!
 * @brief 获取指定路径下的所有文件路径，并按照名称进行升序排序
 * @param path 路径
 * @return 所有文件的完整路径
 */
inline std::vector<std::string> files_in_dir(std::string_view path) {
    if (!std::filesystem::exists(path)) {
        spdlog::error("Directory does not exist, dir:{}", path);
        return {};
    }
    std::vector<std::string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            filenames.push_back(std::filesystem::absolute(entry).string());
        }
    }
    std::ranges::sort(filenames);
    return filenames;
}

/*!
 * @brief 获取指定路径下的指定扩展名的所有文件路径，并按照名称进行升序排序
 * @param path 路径
 * @param extension 包含句点的扩展名，如：.txt
 * @return 所有文件的完整路径
 */
inline std::vector<std::string> files_in_dir(std::string_view path, std::string_view extension) {
    if (!std::filesystem::exists(path)) {
        spdlog::error("Directory does not exist, dir:{}", path);
        return {};
    }
    std::vector<std::string> filenames;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            filenames.push_back(std::filesystem::absolute(entry).string());
        }
    }
    std::ranges::sort(filenames);
    return filenames;
}

template<typename T, std::enable_if_t<sizeof(T)==1>* = nullptr>
std::vector<T> read_binary_file(const std::string &filename) {
    return detail::read_binary_file<std::vector<T>>(filename);
}

template<typename T, std::enable_if_t<std::is_same_v<T, std::string>>* = nullptr>
std::string read_binary_file(const std::string &filename) {
    return detail::read_binary_file<std::string>(filename);
}

inline bool write_binary_file(const std::string &filename, std::span<const char> contents) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        spdlog::error("Open file failed: {}.", filename);
        return false;
    }
    file.write(contents.data(), static_cast<std::streamsize>(contents.size()));
    return true;
}

inline std::string read_text_file(const std::string &filename) {
    const std::ifstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Open file failed: {}.", filename);
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

inline bool write_text_file(const std::string &filename, const std::string &text) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        spdlog::error("Write file failed: {}.", filename);
        return false;
    }
    file.write(text.data(), static_cast<std::streamsize>(text.size()));
    return true;
}

template<typename T>
std::optional<T> parse_json(const std::string &text) {
    if (text.empty()) {
        spdlog::error("Json content is empty");
        return std::nullopt;
    }

    try {
        const auto json_obj = nlohmann::json::parse(text);
        T obj = json_obj.get<T>();
        return obj;
    } catch (std::exception &ex) {
        spdlog::error("Parse json exception:{}, content:{}", ex.what(), text);
        return std::nullopt;
    }
}

template<typename T>
std::optional<T> read_json(const std::string &filename) {
    return parse_json<T>(read_text_file(filename));
}

template<typename T>
bool write_json(const std::string &filename, const T &obj) {
    try {
        const nlohmann::json json_obj = obj;
        const std::string text = json_obj.dump(4);
        return write_text_file(filename, text);
    } catch (std::exception &ex) {
        spdlog::error("Write json exception:{}, filename:{}", ex.what(), filename);
        return false;
    }
}

template<typename T>
T read_json_or_generate_default(const std::string &filename) {
    const std::optional<T> obj_opt = read_json<T>(filename);
    if (obj_opt) {
        return obj_opt.value();
    } else {
        T obj{};
        write_json(filename, obj);
        return obj;
    }
}
