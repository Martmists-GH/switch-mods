#include "imgui_internal.h"
#include "logger/logger.h"
#include "newui/newui.h"

namespace newui {
    NEWUI_ELEMENT(Button) {
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto size = jsonutil::getImVec2(props, "size", ImVec2(0, 0));
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto enabled = jsonutil::getBool(props, "enabled", true);
        auto onClick = jsonutil::getString(props, "onclick", Renderer::PLACEHOLDER);

        ImGui::BeginDisabled(!enabled);
        if (ImGui::ButtonEx(label.data(), size, flags)) {
            renderer.invokeCallback(onClick, "onclick", context);
        }
        ImGui::EndDisabled();
    }

    NEWUI_ELEMENT(Combo) {
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto key = jsonutil::getString(props, "key", "");
        auto preview = jsonutil::getString(props, "preview", "");
        auto flags = jsonutil::getInt(props, "flags", 0);

        if (preview.empty()) {
            if (!key.empty()) {
                auto value = context.value(key, "");

                if (!value.empty()) {
                    for (auto child : node.children()) {
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
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }

            ImGui::EndCombo();
        }

    }

    NEWUI_ELEMENT(Selectable) {
        auto key = jsonutil::getString(props, "key", "");
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto value = jsonutil::getString(props, "value", label);
        auto size = jsonutil::getImVec2(props, "size", ImVec2(0, 0));
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto enabled = jsonutil::getBool(props, "enabled", true);
        auto onClick = jsonutil::getString(props, "onclick", Renderer::PLACEHOLDER);

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
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto key = jsonutil::getString(props, "key", "");
        auto defaultValue = jsonutil::getBool(props, "default", false);
        auto onChange = jsonutil::getString(props, "onchange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

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
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto key = jsonutil::getString(props, "key", "");
        auto defaultValue = jsonutil::getInt(props, "default", 0);
        auto min = jsonutil::getInt(props, "min", 0);
        auto max = jsonutil::getInt(props, "max", 100);
        auto step = jsonutil::getInt(props, "step", 1);
        auto stepFast = jsonutil::getInt(props, "stepfast", 10);
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto onChange = jsonutil::getString(props, "onchange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

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
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto key = jsonutil::getString(props, "key", "");
        auto defaultValue = jsonutil::getInt(props, "default", 0);
        auto min = jsonutil::getInt(props, "min", 0);
        auto max = jsonutil::getInt(props, "max", 100);
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto onChange = jsonutil::getString(props, "onchange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

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
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto key = jsonutil::getString(props, "key", "");
        auto defaultValue = jsonutil::getFloat(props, "default", 0);
        auto min = jsonutil::getFloat(props, "min", 0);
        auto max = jsonutil::getFloat(props, "max", 100);
        auto step = jsonutil::getFloat(props, "step", 1);
        auto stepFast = jsonutil::getFloat(props, "stepfast", 10);
        auto format = jsonutil::getString(props, "format", "%.3f");
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto onChange = jsonutil::getString(props, "onchange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

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
        auto label = jsonutil::getString(props, "label", "<% ERROR: Missing attribute 'label' %>");
        auto key = jsonutil::getString(props, "key", "");
        auto defaultValue = jsonutil::getFloat(props, "default", 0);
        auto min = jsonutil::getFloat(props, "min", 0);
        auto max = jsonutil::getFloat(props, "max", 100);
        auto format = jsonutil::getString(props, "format", "%.3f");
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto onChange = jsonutil::getString(props, "onchange", Renderer::PLACEHOLDER);
        auto enabled = jsonutil::getBool(props, "enabled", true);

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
