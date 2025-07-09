#pragma once

#include <map>
#include <string>
#include <type_traits>

#include <nlohmann/json.hpp>

template <typename KTy, typename VTy,
    std::enable_if_t<std::is_integral_v<KTy> || std::is_floating_point_v<KTy>>* = nullptr>
std::map<std::string, VTy> to_str_key_map(std::map<KTy, VTy> const& input) {
    std::map<std::string, VTy> output;
    for (const auto& [key, val] : input) {
        output.emplace(std::to_string(key), val);
    }
    return output;
}

template <typename KTy, typename VTy,
    std::enable_if_t<std::is_integral_v<KTy> || std::is_floating_point_v<KTy>>* = nullptr>
std::map<KTy, VTy> from_str_key_map(const std::map<std::string, VTy>& input) {
    std::map<KTy, VTy> output;
    for (const auto& [key, val] : input) {
        if constexpr (std::is_integral_v<KTy>) {
            output.emplace(std::stoll(key), val);
        } else {
            output.emplace(std::stold(key), val);
        }
    }
    return output;
}


/*!
 * @brief 将JSON对象转换为数值作为Key的map
 * @tparam TMap 转换结果map类型（数值作为key）
 * @param j JSON对象
 * @param obj 转换结果map对象（数值作为key）
 * @param name 对象名称
 */
template<typename TMap>
void from_json_str_key_map(const nlohmann::json &j, TMap &obj, const std::string &name) {
    std::map<std::string, typename TMap::mapped_type> data;
    j.at(name).get_to(data);
    obj = from_str_key_map<typename TMap::key_type>(data);
}

/*!
 * @brief 将JSON对象转换为数值作为Key的map
 * @param json JSON对象
 * @param obj 转换结果对象，obj中包含field
 * @param field obj对象中的某个字段
 */
#define FROM_JSON_STR_KEY_MAP(json, obj, field) from_json_str_key_map(json, obj.field, #field);
