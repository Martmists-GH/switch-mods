#pragma once

#include <unordered_map>
#include <unordered_set>

#include "pugixml.hpp"
#include "inja/inja.hpp"
#include "nlohmann/json.hpp"

namespace newui {
    class Renderer;

    using RendererCallback = std::function<void(Renderer&, const pugi::xml_node&, const nlohmann::json&, nlohmann::json&)>;

    template <typename T>
    struct __FunctionCallbackTraits {
        using type = std::function<void(nlohmann::json&, T, T)>;
    };

    template <>
    struct __FunctionCallbackTraits<void> {
        using type = std::function<void(nlohmann::json&)>;
    };

    template <typename T>
    using FunctionCallback = __FunctionCallbackTraits<T>::type;

    static nlohmann::json parseAttrs(const pugi::xml_node& node) {
        auto props = nlohmann::json::object();
        for (const auto& attr : node.attributes()) {
            props[attr.name()] = attr.value();
        }
        return props;
    }

    class Renderer {
        std::string uiTemplate = "<Text content=\"<% ERROR: Renderer not initialized. %>\" />";
        nlohmann::json context = {};
        pugi::xml_document doc = {};
        std::unordered_map<std::string, RendererCallback> renderers = {};

        std::unordered_map<std::string, FunctionCallback<void>> callbacksVoid = {};
        std::unordered_map<std::string, FunctionCallback<bool>> callbacksBoolean = {};
        std::unordered_map<std::string, FunctionCallback<int>> callbacksInt = {};
        std::unordered_map<std::string, FunctionCallback<float>> callbacksFloat = {};
        std::unordered_map<std::string, FunctionCallback<std::string&>> callbacksString = {};
        Renderer* parent = nullptr;

        bool m_needsUpdate = false;

        void initContext();
        void initTypes();
        void initCallbacks();

        template <typename T>
        auto& getCallbackMap() {
            if constexpr (std::is_void_v<T>) return callbacksVoid;
            else if constexpr (std::is_same_v<T, bool>) return callbacksBoolean;
            else if constexpr (std::is_same_v<T, int>) return callbacksInt;
            else if constexpr (std::is_same_v<T, float>) return callbacksFloat;
            else if constexpr (std::is_same_v<T, std::string&>) return callbacksString;
        }

    public:
        static constexpr const char* PLACEHOLDER = "$PLACEHOLDER$";

        Renderer(Renderer* parent = nullptr);

        Renderer& root();

        template <typename T>
        void registerCallback(const std::string& name, FunctionCallback<T> callback) {
            root().getCallbackMap<T>()[name] = std::move(callback);
        }
        void registerRenderer(const std::string& type, RendererCallback callback);

        template <typename T>
        bool hasCallback(const std::string& name) {
            return root().getCallbackMap<T>().contains(name);
        }

        template <typename T>
        void invokeCallback(const std::string& name, nlohmann::json& ctx, T before, T after) {
            root().getCallbackMap<T>()[name](ctx, before, after);
        }

        void invokeCallback(const std::string& name, nlohmann::json& ctx) {
            root().callbacksVoid[name](ctx);
        }

        void init(const char* path);
        bool needsUpdate();
        void markNeedsUpdate();

        void render();
        void render(nlohmann::json& ctx);
        void render(const pugi::xml_node& node, nlohmann::json& ctx);

        // AVOID USE, ONLY DESIGNED FOR <Include /> TAGS
        std::unordered_map<std::string, Renderer> includeCache = {};
    };
}
