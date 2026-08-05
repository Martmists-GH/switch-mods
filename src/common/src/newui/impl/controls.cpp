#include "imgui_internal.h"
#include "logger/logger.h"
#include "newui/newui.h"

namespace newui {
    NEWUI_ELEMENT(Button) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto size = jsonutil::getImVec2(props, "size", ImVec2(0, 0));
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto enabled = jsonutil::getBool(props, "enabled", true);

        auto onClick = props.value("onclick", Renderer::PLACEHOLDER);

        if (!renderer.hasCallback<void>(onClick)) {
            ImGui::BeginDisabled();
            auto content = std::format("<% ERROR: Unknown 'onclick' callback '{}' %>", onClick);
            ImGui::ButtonEx(content.c_str(), size, flags);
            ImGui::EndDisabled();
        } else {
            ImGui::BeginDisabled(!enabled);
            if (ImGui::ButtonEx(label.c_str(), size, flags)) {
                renderer.invokeCallback(onClick, context);
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(Selectable) {
        auto key = props.value("key", "");
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto value = props.value("value", label);
        auto size = jsonutil::getImVec2(props, "size", ImVec2(0, 0));
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto enabled = jsonutil::getBool(props, "enabled", true);
        auto onClick = props.value("onclick", Renderer::PLACEHOLDER);

        if (!renderer.hasCallback<std::string&>(onClick)) {
            ImGui::BeginDisabled();
            auto content = std::format("<% ERROR: Unknown 'onclick' callback '{}' %>", onClick);
            ImGui::Selectable(content.c_str());
            ImGui::EndDisabled();
        }

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::Selectable("<% ERROR: Missing attribute 'key' %>");
            ImGui::EndDisabled();
        } else {
            ImGui::BeginDisabled(!enabled);
            auto current = context.value(key, "");
            if (ImGui::Selectable(label.c_str(), current == value, flags, size)) {
                renderer.invokeCallback<std::string&>(onClick, context, current, value);
                context[key] = value;
                renderer.markNeedsUpdate();
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(Checkbox) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = props.value("key", "");
        auto defaultValue = jsonutil::getBool(props, "default", false);
        auto onChange = props.value("onChange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

        if (!renderer.hasCallback<bool>(onChange)) {
            ImGui::BeginDisabled();
            auto content = std::format("<% ERROR: Unknown 'onchange' callback '{}' %>", onChange);
            ImGui::Selectable(content.c_str());
            ImGui::EndDisabled();
        }

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::Checkbox("<% ERROR: Missing attribute 'key' %>", &defaultValue);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getBool(context, key, defaultValue);
            auto value = current;
            ImGui::BeginDisabled(!enabled);
            if (ImGui::Checkbox(label.c_str(), &value)) {
                renderer.invokeCallback(onChange, context, current, value);
                context[key] = value;
                renderer.markNeedsUpdate();
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(InputInt) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = props.value("key", "");
        auto defaultValue = jsonutil::getInt(props, "default", 0);
        auto min = jsonutil::getInt(props, "min", 0);
        auto max = jsonutil::getInt(props, "max", 100);
        auto step = jsonutil::getInt(props, "step", 1);
        auto stepFast = jsonutil::getInt(props, "stepfast", 100);
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto onChange = props.value("onChange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

        if (!renderer.hasCallback<int>(onChange)) {
            ImGui::BeginDisabled();
            auto content = std::format("<% ERROR: Unknown 'onchange' callback '{}' %>", onChange);
            ImGui::Selectable(content.c_str());
            ImGui::EndDisabled();
        }

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::InputInt("<% ERROR: Missing attribute 'key' %>", &defaultValue);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getInt(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::InputInt(label.c_str(), &value, step, stepFast, flags)) {
                if (value < min) {
                    value = min;
                } else if (value > max) {
                    value = max;
                }

                if (value != current) {
                    renderer.invokeCallback(onChange, context, current, value);
                    context[key] = value;
                    renderer.markNeedsUpdate();
                }
            }
            ImGui::EndDisabled();
        }
    }

    NEWUI_ELEMENT(InputFloat) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto key = props.value("key", "");
        auto defaultValue = jsonutil::getFloat(props, "default", 0);
        auto min = jsonutil::getFloat(props, "min", 0);
        auto max = jsonutil::getFloat(props, "max", 100);
        auto step = jsonutil::getFloat(props, "step", 1);
        auto stepFast = jsonutil::getFloat(props, "stepfast", 100);
        auto format = props.value("format", "%.3f");
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto onChange = props.value("onChange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

        if (!renderer.hasCallback<float>(onChange)) {
            ImGui::BeginDisabled();
            auto content = std::format("<% ERROR: Unknown 'onchange' callback '{}' %>", onChange);
            ImGui::Selectable(content.c_str());
            ImGui::EndDisabled();
        }

        if (key.empty()) {
            ImGui::BeginDisabled();
            ImGui::InputFloat("<% ERROR: Missing attribute 'key' %>", &defaultValue);
            ImGui::EndDisabled();
        } else {
            auto current = jsonutil::getFloat(context, key, defaultValue);
            auto value = current;

            ImGui::BeginDisabled(!enabled);
            if (ImGui::InputFloat(label.c_str(), &value, step, stepFast, format.c_str(), flags)) {
                if (value < min) {
                    value = min;
                } else if (value > max) {
                    value = max;
                }

                if (value != current) {
                    renderer.invokeCallback(onChange, context, current, value);
                    context[key] = value;
                    renderer.markNeedsUpdate();
                }
            }
            ImGui::EndDisabled();
        }
    }
}
