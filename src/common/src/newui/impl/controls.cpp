#include "imgui_internal.h"
#include "logger/logger.h"
#include "newui/newui.h"

namespace newui {
    NEWUI_ELEMENT(Button) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto size = node.get<ImVec2>("size", ImVec2(0, 0));
        auto flags = node.get<int>("flags", 0);
        auto enabled = node.get<bool>("enabled", true);
        auto onClick = node.get<std::string_view>("onclick", Renderer::PLACEHOLDER);

        ImGui::BeginDisabled(!enabled);
        if (ImGui::ButtonEx(label.data(), size, flags)) {
            renderer.invokeCallback(onClick, "onclick", context);
        }
        ImGui::EndDisabled();
    }

    NEWUI_ELEMENT(Combo) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = node.get<std::string_view>("key", "");
        auto preview = node.get<std::string_view>("preview", "");
        auto flags = node.get<int>("flags", 0);

        if (preview.empty()) {
            if (!key.empty()) {
                auto value = jsonutil::getString(context, key, "");

                if (!value.empty()) {
                    for (auto& cachedChild : node.children) {
                        auto& child = cachedChild.node;
                        if (child.type() == pugi::node_element && strcasecmp(child.name(), "Selectable") == 0) {
                            auto val = child.attribute("value");
                            if (val.empty()) {
                                val = child.attribute("label");
                            }
                            if (val.empty()) continue;

                            if (value == val.value()) {
                                preview = child.attribute("label").value();
                                break;
                            }
                        }
                    }
                }
            } else {
                preview = "<% ERROR: Missing attribute 'preview' %>";
            }
        }

        if (ImGui::BeginCombo(label.data(), preview.data(), flags)) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }

            ImGui::EndCombo();
        }

    }

    NEWUI_ELEMENT(Selectable) {
        auto key = node.get<std::string_view>("key", "");
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto value = node.get<std::string_view>("value", label);
        auto size = node.get<ImVec2>("size", ImVec2(0, 0));
        auto flags = node.get<int>("flags", 0);
        auto enabled = node.get<bool>("enabled", true);
        auto onClick = node.get<std::string_view>("onclick", Renderer::PLACEHOLDER);

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::Selectable("<% ERROR: Missing attribute 'key' %>", false, flags, size);
            ImGui::EndDisabled();
        } else {
            ImGui::BeginDisabled(!enabled);
            auto current = jsonutil::getString(context, key, "");
            if (ImGui::Selectable(label.data(), current == value, flags, size)) {
                renderer.invokeCallback(onClick, "onclick", context, current, value);
                jsonutil::assignPath(context, key, value);
                renderer.markNeedsUpdate();
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(Checkbox) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = node.get<std::string_view>("key", "");
        auto defaultValue = node.get<bool>("default", false);
        auto onChange = node.get<std::string_view>("onchange", Renderer::PLACEHOLDER);
        auto enabled = node.get<bool>("enabled", true);

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::Checkbox("<% ERROR: Missing attribute 'key' %>", &defaultValue);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getBool(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::Checkbox(label.data(), &value)) {
                renderer.invokeCallback(onChange, "onchange", context, current, value);
                jsonutil::assignPath(context, key, value);
                renderer.markNeedsUpdate();
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(InputInt) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = node.get<std::string_view>("key", "");
        auto defaultValue = node.get<int>("default", 0);
        auto min = node.get<int>("min", 0);
        auto max = node.get<int>("max", 100);
        auto step = node.get<int>("step", 1);
        auto stepFast = node.get<int>("stepfast", 10);
        auto flags = node.get<int>("flags", 0);
        auto onChange = node.get<std::string_view>("onchange", Renderer::PLACEHOLDER);
        auto enabled = node.get<bool>("enabled", true);

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::InputInt("<% ERROR: Missing attribute 'key' %>", &defaultValue, step, stepFast, flags);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getInt(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::InputInt(label.data(), &value, step, stepFast, flags)) {
                if (value < min) {
                    value = min;
                } else if (value > max) {
                    value = max;
                }

                if (value != current) {
                    renderer.invokeCallback(onChange, "onchange", context, current, value);
                    jsonutil::assignPath(context, key, value);
                    renderer.markNeedsUpdate();
                }
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(SliderInt) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = node.get<std::string_view>("key", "");
        auto defaultValue = node.get<int>("default", 0);
        auto min = node.get<int>("min", 0);
        auto max = node.get<int>("max", 100);
        auto flags = node.get<int>("flags", 0);
        auto onChange = node.get<std::string_view>("onchange", Renderer::PLACEHOLDER);
        auto enabled = node.get<bool>("enabled", true);

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::SliderInt("<% ERROR: Missing attribute 'key' %>", &defaultValue, min, max, "%d", flags);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getInt(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::SliderInt(label.data(), &value, min, max, "%d", flags)) {
                if (value != current) {
                    renderer.invokeCallback(onChange, "onchange", context, current, value);
                    jsonutil::assignPath(context, key, value);
                    renderer.markNeedsUpdate();
                }
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(InputFloat) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = node.get<std::string_view>("key", "");
        auto defaultValue = node.get<float>("default", 0);
        auto min = node.get<float>("min", 0);
        auto max = node.get<float>("max", 100);
        auto step = node.get<float>("step", 1);
        auto stepFast = node.get<float>("stepfast", 10);
        auto format = node.get<std::string_view>("format", "%.3f");
        auto flags = node.get<int>("flags", 0);
        auto onChange = node.get<std::string_view>("onchange", Renderer::PLACEHOLDER);
        auto enabled = node.get<bool>("enabled", true);

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::InputFloat("<% ERROR: Missing attribute 'key' %>", &defaultValue, step, stepFast, format.data(), flags);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getFloat(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::InputFloat(label.data(), &value, step, stepFast, format.data(), flags)) {
                if (value < min) {
                    value = min;
                } else if (value > max) {
                    value = max;
                }

                if (value != current) {
                    renderer.invokeCallback(onChange, "onchange", context, current, value);
                    jsonutil::assignPath(context, key, value);
                    renderer.markNeedsUpdate();
                }
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(SliderFloat) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = node.get<std::string_view>("key", "");
        auto defaultValue = node.get<float>("default", 0);
        auto min = node.get<float>("min", 0);
        auto max = node.get<float>("max", 100);
        auto format = node.get<std::string_view>("format", "%.3f");
        auto flags = node.get<int>("flags", 0);
        auto onChange = node.get<std::string_view>("onchange", Renderer::PLACEHOLDER);
        auto enabled = node.get<bool>("enabled", true);

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::SliderFloat("<% ERROR: Missing attribute 'key' %>", &defaultValue, min, max, format.data(), flags);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getFloat(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::SliderFloat(label.data(), &value, min, max, format.data(), flags)) {
                if (value != current) {
                    renderer.invokeCallback(onChange, "onchange", context, current, value);
                    jsonutil::assignPath(context, key, value);
                    renderer.markNeedsUpdate();
                }
            }
            ImGui::EndDisabled();
        }
    }
}
