#pragma once

#include <charconv>
#include "imgui.h"
#include "nlohmann/json.hpp"

namespace jsonutil {
    static float getFloat(const nlohmann::json& j, const std::string& key, float defaultValue) {
        if (!j.is_object() || !j.contains(key) || j[key].is_null()) return defaultValue;
        const auto& val = j[key];
        if (val.is_number()) return val.get<float>();
        if (val.is_string()) {
            const auto& str = val.get_ref<const std::string&>();
            if (str.empty()) return defaultValue;
            char* end = nullptr;
            float result = std::strtof(str.c_str(), &end);
            if (end != str.c_str()) return result;
        }

        return defaultValue;
    }

    static int getInt(const nlohmann::json& j, const std::string& key, int defaultValue) {
        if (!j.is_object() || !j.contains(key) || j[key].is_null()) return defaultValue;
        const auto& val = j[key];
        if (val.is_number_integer() || val.is_number_unsigned()) return val.get<int>();
        if (val.is_number_float()) return static_cast<int>(val.get<float>());
        if (val.is_string()) {
            const auto& str = val.get_ref<const std::string&>();
            if (str.empty()) return defaultValue;
            int result = 0;
            auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
            if (ec == std::errc {}) return result;
        }
        return defaultValue;
    }

    static bool getBool(const nlohmann::json& j, const std::string& key, bool defaultValue) {
        if (!j.is_object() || !j.contains(key) || j[key].is_null()) return defaultValue;
        const auto& val = j[key];
        if (val.is_boolean()) return val.get<bool>();
        if (val.is_number_integer() || val.is_number_unsigned()) return val.get<int>() != 0;
        if (val.is_number_float()) return static_cast<int>(val.get<float>()) != 0;
        if (val.is_string()) {
            const auto& str = val.get_ref<const std::string&>();
            if (str.empty()) return defaultValue;
            if (str == "true") return true;
            if (str == "false") return false;
            int result = 0;
            auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);
            if (ec == std::errc {}) return result != 0;
        }
        return defaultValue;
    }

    static ImVec2 getImVec2(const nlohmann::json& j, const std::string& key, ImVec2 defaultValue) {
        if (!j.is_object() || !j.contains(key) || j[key].is_null()) return defaultValue;
        const auto& val = j[key];
        if (val.is_number()) {
            float z = val.get<float>();
            return {z, z};
        }
        if (val.is_string()) {
            const auto& str = val.get_ref<const std::string&>();
            if (str.empty()) return defaultValue;
            size_t commaPos = str.find(',');
            if (commaPos != std::string::npos) {
                std::string xStr = str.substr(0, commaPos);
                std::string yStr = str.substr(commaPos + 1);
                char* endX = nullptr;
                char* endY = nullptr;
                float x = std::strtof(xStr.c_str(), &endX);
                float y = std::strtof(yStr.c_str(), &endY);
                if (endX != xStr.c_str() && endY != yStr.c_str()) {
                    return {x, y};
                }
            } else {
                char* end = nullptr;
                float z = std::strtof(str.c_str(), &end);
                if (end != str.c_str()) {
                    return {z, z};
                }
            }
        }
        return defaultValue;
    }
}
