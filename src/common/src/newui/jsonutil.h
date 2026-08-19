#pragma once

#include <charconv>
#include "imgui.h"
#include "pugixml.hpp"
#include "nlohmann/json.hpp"
#include "logger/logger.h"

namespace jsonutil {
    static const nlohmann::json* loadPath(const nlohmann::json& j, std::string_view path) {
        const nlohmann::json* current = &j;
        std::string_view rem = path;

        size_t offset = 0;
        while ((offset = rem.find('.')) != std::string_view::npos) {
            auto part = rem.substr(0, offset);

            if (!current->is_object()) {
                return nullptr;
            }

            auto it = current->find(std::string(part));
            if (it == current->end()) {
                return nullptr;
            }

            current = &(*it);
            rem.remove_prefix(offset + 1);
        }

        if (!current->is_object()) {
            return nullptr;
        }

        auto it = current->find(std::string(rem));
        if (it == current->end()) {
            return nullptr;
        }

        return &(*it);
    }

    template <typename T>
    static void assignPath(nlohmann::json& j, std::string_view path, T&& value) {
        nlohmann::json* current = &j;
        std::string_view rem = path;

        size_t offset = 0;
        while ((offset = rem.find('.')) != std::string_view::npos) {
            auto part = rem.substr(0, offset);
            current = &((*current)[std::string(part)]);
            rem.remove_prefix(offset + 1);
        }
        (*current)[std::string(rem)] = std::forward<T>(value);
    }

    static std::string_view getString(const nlohmann::json& j, std::string_view key, std::string_view defaultValue) {
        if (!j.is_object()) return defaultValue;
        auto path = loadPath(j, key);
        if (path == nullptr) return defaultValue;
        auto& val = *path;
        if (val.is_string()) {
            return val.get_ref<const std::string&>();
        }
        return defaultValue;
    }

    static std::string_view getString(const pugi::xml_node& node, std::string_view key, std::string_view defaultValue) {
        auto attr = node.attribute(key);
        if (!attr || !attr.value()[0]) return defaultValue;
        return attr.value();
    }

    static float asFloat(std::string_view value, float defaultValue) {
        char* end = nullptr;
        float result = std::strtof(value.data(), &end);
        if (end != value.data()) return result;
        return defaultValue;
    }

    static float getFloat(const nlohmann::json& j, std::string_view key, float defaultValue) {
        if (!j.is_object()) return defaultValue;
        auto path = loadPath(j, key);
        if (path == nullptr) return defaultValue;
        auto& val = *path;
        if (val.is_number()) return val.get<float>();
        if (val.is_string()) {
            auto& str = val.get_ref<const std::string&>();
            return asFloat(str, defaultValue);
        }

        return defaultValue;
    }

    static float getFloat(const pugi::xml_node& node, std::string_view key, float defaultValue) {
        auto attr = node.attribute(key);
        if (!attr || !attr.value()[0]) return defaultValue;
        return asFloat(attr.value(), defaultValue);
    }

    inline int asInt(std::string_view value, int defaultValue) {
        char* end = nullptr;
        int base = 10;
        if (value.starts_with("0x")) {
            value.remove_prefix(2);
            base = 16;
        }
        int result = std::strtol(value.data(), &end, base);
        if (end != value.data()) return result;
        return defaultValue;
    }

    static int getInt(const nlohmann::json& j, std::string_view key, int defaultValue) {
        if (!j.is_object()) return defaultValue;
        auto path = loadPath(j, key);
        if (path == nullptr) return defaultValue;
        auto& val = *path;
        if (val.is_number_integer() || val.is_number_unsigned()) return val.get<int>();
        if (val.is_number_float()) return static_cast<int>(val.get<float>());
        if (val.is_string()) {
            auto& str = val.get_ref<const std::string&>();
            return asInt(str, defaultValue);
        }
        return defaultValue;
    }

    static int getInt(const pugi::xml_node& node, std::string_view key, int defaultValue) {
        auto attr = node.attribute(key);
        if (!attr || !attr.value()[0]) return defaultValue;
        return asInt(attr.value(), defaultValue);
    }

    inline bool asBool(std::string_view value, bool defaultValue) {
        if (value == "true") return true;
        if (value == "false") return false;
        return defaultValue;
    }

    static bool getBool(const nlohmann::json& j, std::string_view key, bool defaultValue) {
        if (!j.is_object()) return defaultValue;
        auto path = loadPath(j, key);
        if (path == nullptr) return defaultValue;
        const auto& val = *path;
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

    static bool getBool(const pugi::xml_node& node, std::string_view key, bool defaultValue) {
        auto attr = node.attribute(key);
        if (!attr || !attr.value()[0]) return defaultValue;
        return asBool(attr.value(), defaultValue);
    }

    inline ImVec2 asImVec2(std::string_view value, ImVec2 defaultValue) {
        auto commaPos = value.find(',');
        if (commaPos != std::string::npos) {
            auto xStr = value.substr(0, commaPos);
            auto yStr = value.substr(commaPos + 1);
            char* endX = nullptr;
            char* endY = nullptr;
            float x = std::strtof(xStr.data(), &endX);
            float y = std::strtof(yStr.data(), &endY);
            if (endX != xStr.data() && endY != yStr.data()) {
                return {x, y};
            }
        } else {
            char* end = nullptr;
            float z = std::strtof(value.data(), &end);
            if (end != value.data()) {
                return {z, z};
            }
        }
        return defaultValue;
    }

    static ImVec2 getImVec2(const nlohmann::json& j, std::string_view key, ImVec2 defaultValue) {
        if (!j.is_object()) return defaultValue;
        auto path = loadPath(j, key);
        if (path == nullptr) return defaultValue;
        const auto& val = *path;
        if (val.is_array() && val.size() == 2) {
            return { val[0].get<float>(), val[1].get<float>() };
        }
        if (val.is_number()) {
            auto z = val.get<float>();
            return {z, z};
        }
        if (val.is_string()) {
            auto& str = val.get_ref<const std::string&>();
            return asImVec2(str, defaultValue);
        }
        return defaultValue;
    }

    static ImVec2 getImVec2(const pugi::xml_node& node, std::string_view key, ImVec2 defaultValue) {
        auto attr = node.attribute(key);
        if (!attr || !attr.value()[0]) return defaultValue;
        return asImVec2(attr.value(), defaultValue);
    }

    inline ImColor asImColor(std::string_view value, ImColor defaultValue) {
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;
        bool matchesRGB = false;

        if (std::sscanf(value.data(), "rgba ( %f , %f , %f , %f )", &r, &g, &b, &a) == 4 ||
            std::sscanf(value.data(), "RGBA ( %f , %f , %f , %f )", &r, &g, &b, &a) == 4) {
            matchesRGB = true;
        } else if (std::sscanf(value.data(), "rgb ( %f , %f , %f )", &r, &g, &b) == 3 ||
                   std::sscanf(value.data(), "RGB ( %f , %f , %f )", &r, &g, &b) == 3) {
            a = 1.0f;
            matchesRGB = true;
        }

        if (matchesRGB) {
            return {r, g, b, a};
        }

        auto sv = std::string_view(value);
        if (sv.front() == '#') {
            sv.remove_prefix(1);
        }

        std::string hex;
        if (sv.length() == 3 || sv.length() == 4) {
            hex.reserve(sv.length() * 2);
            for (auto c : sv) {
                hex.push_back(c);
                hex.push_back(c);
            }
            sv = hex;
        }

        if (sv.length() == 6 || sv.length() == 8) {
            char* end;
            auto num = std::strtol(value.data(), &end, 16);
            if (end == value.data()) return defaultValue;

            uint8_t alpha, red, green, blue;

            if (sv.length() == 6) {
                red = static_cast<uint8_t>((num >> 16) & 0xFF);
                green = static_cast<uint8_t>((num >> 8) & 0xFF);
                blue = static_cast<uint8_t>(num & 0xFF);
                alpha = 255;
            } else {
                alpha = static_cast<uint8_t>((num >> 24) & 0xFF);
                red = static_cast<uint8_t>((num >> 16) & 0xFF);
                green = static_cast<uint8_t>((num >> 8) & 0xFF);
                blue = static_cast<uint8_t>(num & 0xFF);
            }
            return {red, green, blue, alpha};
        }

        return defaultValue;
    }

    static ImColor getImColor(const nlohmann::json& j, std::string_view key, ImColor defaultValue) {
        if (!j.is_object()) return defaultValue;
        auto path = loadPath(j, key);
        if (path == nullptr) return defaultValue;
        const auto& val = *path;
        if (val.is_array() && val.size() == 4) {
            return { val[0].get<float>(), val[1].get<float>(), val[2].get<float>(), val[3].get<float>() };
        }
        if (val.is_number()) {
            auto num = val.get<uint32_t>();
            uint8_t a = ((num >> 24) & 0xFF);
            uint8_t r = ((num >> 16) & 0xFF);
            uint8_t g = ((num >> 8) & 0xFF);
            uint8_t b = (num & 0xFF);
            if (a == 0 && num != 0) {
                a = 255;
            }
            return {r, g, b, a};
        }
        if (val.is_string()) {
            const auto& str = val.get_ref<const std::string&>();
            if (str.empty()) return defaultValue;
            return asImColor(str, defaultValue);
        }
        return defaultValue;
    }

    static ImColor getImColor(const pugi::xml_node& node, std::string_view key, ImColor defaultValue) {
        auto attr = node.attribute(key);
        if (!attr || !attr.value()[0]) return defaultValue;
        return asImColor(attr.value(), defaultValue);
    }
}
