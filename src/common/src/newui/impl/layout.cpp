#include "imgui.h"
#include "logger/logger.h"
#include "newui/newui.h"
#include "util/InputUtil.h"

namespace newui {
    NEWUI_ELEMENT(Root) {
        auto i = 0;
        for (const auto& child : node.children()) {
            ImGui::PushID(i++);
            renderer.render(child, context);
            ImGui::PopID();
        }
    }

    NEWUI_ELEMENT(Window) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto flags = jsonutil::getInt(props, "flags", ImGuiWindowFlags_NoCollapse);
        auto initialSize = jsonutil::getImVec2(props, "size", ImVec2(0, 0));
        auto initialPos = jsonutil::getImVec2(props, "position", ImVec2(0, 0));
        auto open = props.value("open", "");
        auto sticky = jsonutil::getBool(props, "sticky", false);

        if (!sticky && open.empty() && !InputUtil::isInputToggled()) return;

        if (initialPos.x != 0 || initialPos.y != 0) {
            ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
        }

        if (initialSize.x != 0 && initialSize.y != 0) {
            ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
        }

        if (open.empty()) {
            if (ImGui::Begin(label.c_str(), nullptr, flags)) {
                for (const auto& child : node.children()) {
                    renderer.render(child, context);
                }
            }
            ImGui::End();

        } else {
            // Logger::log("Getting %s from %s: %d", open.c_str(), to_string(context).c_str(), jsonutil::getBool(context, open, false));
            auto current = jsonutil::getBool(context, open, false);
            auto value = current;

            if (!current) return;

            if (ImGui::Begin(label.c_str(), &value, flags)) {
                for (const auto& child : node.children()) {
                    renderer.render(child, context);
                }
            }
            ImGui::End();

            if (!value) {
                context[open] = value;
                renderer.markNeedsUpdate();
            }
        }

    }

    NEWUI_ELEMENT(Row) {
        auto offset = jsonutil::getFloat(props, "offset", 0.0f);
        auto spacing = jsonutil::getFloat(props, "spacing", -1.0f);
        float itemSpacing = (spacing >= 0.0f) ? spacing : ImGui::GetStyle().ItemSpacing.x;

        float totalWeight = 0.0f;
        std::vector<pugi::xml_node> children;
        bool hasZeroWeight = false;

        for (const auto& child : node.children()) {
            if (child.type() != pugi::node_element) continue;
            children.emplace_back(child);
            auto w = jsonutil::getFloat(parseAttrs(child), "weight", 0.0f);
            totalWeight += w;
            if (w <= 0.0f) hasZeroWeight = true;
        }

        if (children.empty()) return;

        if (totalWeight == 0.0f) {
            auto i = 0;
            for (auto child : children) {
                ImGui::PushID(i);
                if (i++ != 0) ImGui::SameLine(offset, spacing);
                renderer.render(child, context);
                ImGui::PopID();
            }
            return;
        }

        if (hasZeroWeight) {
            ImGui::TextUnformatted("<% ERROR: Element in Row had weight of 0 %>");
        }

        float availWidth = ImGui::GetContentRegionAvail().x;
        float availHeight = ImGui::GetContentRegionAvail().y;
        float totalSpacing = itemSpacing * static_cast<float>(children.size() - 1);
        float usableWidth = std::max(0.0f, availWidth - totalSpacing);

        auto i = 0;
        for (auto child : children) {
            float childWeight = jsonutil::getFloat(parseAttrs(child), "weight", 0.0f);
            float allocatedWidth = (totalWeight > 0.0f) ? (usableWidth * (childWeight / totalWeight)) : 0.0f;

            ImGui::PushID(i);
            if (i != 0) ImGui::SameLine(offset, itemSpacing);
            std::string childId = std::format("##row_slot_{}", i++);
           if (ImGui::BeginChild(childId.c_str(), ImVec2(allocatedWidth, availHeight), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
               renderer.render(child, context);
           }
           ImGui::EndChild();

           ImGui::PopID();
        }
    }

    NEWUI_ELEMENT(Column) {
        auto userSpacing = jsonutil::getFloat(props, "spacing", -1.0f);
        float itemSpacing = (userSpacing >= 0.0f) ? userSpacing : ImGui::GetStyle().ItemSpacing.y;

        float totalWeight = 0.0f;
        std::vector<pugi::xml_node> children;
        bool hasZeroWeight = false;

        for (auto child : node.children()) {
            if (child.type() != pugi::node_element) continue;
            children.emplace_back(child);
            auto w = jsonutil::getFloat(parseAttrs(child), "weight", 0.0f);
            totalWeight += w;
            if (w <= 0.0f) hasZeroWeight = true;
        }

        if (children.empty()) return;

        if (totalWeight == 0.0f) {
            for (auto& child : children) {
                renderer.render(child, context);
            }
            return;
        }

        if (hasZeroWeight) {
            ImGui::TextUnformatted("<% ERROR: Element in Column had weight of 0 %>");
        }

        float availHeight = ImGui::GetContentRegionAvail().y;
        float totalSpacing = itemSpacing * static_cast<float>(children.size() - 1);
        float usableHeight = std::max(0.0f, availHeight - totalSpacing);

        auto i = 0;
        for (auto& child : children) {
            float childWeight = jsonutil::getFloat(parseAttrs(child), "weight", 1.0f);
            float allocatedHeight = (totalWeight > 0.0f) ? (usableHeight * (childWeight / totalWeight)) : 0.0f;

            ImGui::PushID(i);
            std::string childId = std::format("##col_slot_{}", i);
            if (ImGui::BeginChild(childId.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, allocatedHeight), 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
                renderer.render(child, context);
            }
            ImGui::EndChild();
            ImGui::PopID();

            if (++i < children.size() && itemSpacing > 0.0f) {
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemSpacing);
            }
        }
    }

    NEWUI_ELEMENT(Child) {
        auto id = props.value("id", node.path());
        auto size = jsonutil::getImVec2(props, "size", ImVec2(0, 0));
        auto flags = jsonutil::getInt(props, "flags", 0);
        auto windowFlags = jsonutil::getInt(props, "windowflags", 0);

        if (ImGui::BeginChild(id.c_str(), size, flags, windowFlags)) {
            auto i = 0;
            for (const auto& child : node.children()) {
                ImGui::PushID(i);
                renderer.render(child, context);
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }

    NEWUI_ELEMENT(CollapsingHeader) {
        auto label = props.value("label", "<% ERROR: Missing attribute 'label' %>");
        auto flags = jsonutil::getInt(props, "flags", 0);

        if (ImGui::CollapsingHeader(label.c_str(), flags)) {
            for (const auto& child : node.children()) {
                renderer.render(child, context);
            }
        }
    }
}
