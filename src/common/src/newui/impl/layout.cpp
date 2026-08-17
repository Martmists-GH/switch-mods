#include "imgui.h"
#include "imgui_internal.h"
#include "logger/logger.h"
#include "newui/newui.h"
#include "util/InputUtil.h"

namespace newui {
    NEWUI_ELEMENT(Root) {
        auto i = 0;
        for (const auto& child : node.children()) {
            ImGui::PushID(child.internal_object());
            renderer.render(child, context);
            ImGui::PopID();
        }
    }

    NEWUI_ELEMENT(Window) {
        auto label = jsonutil::getString(node, "label", "<% ERROR: Missing attribute 'label' %>");
        auto flags = jsonutil::getInt(node, "flags", ImGuiWindowFlags_NoCollapse);
        auto initialSize = jsonutil::getImVec2(node, "size", ImVec2(0, 0));
        auto initialPos = jsonutil::getImVec2(node, "position", ImVec2(0, 0));
        auto open = jsonutil::getString(node, "open", "");
        auto sticky = jsonutil::getBool(node, "sticky", false);

        if (!sticky && open.empty() && !InputUtil::isInputToggled()) return;

        if (initialPos.x != 0 || initialPos.y != 0) {
            ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
        }

        if (initialSize.x != 0 && initialSize.y != 0) {
            ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
        }

        if (open.empty()) {
            if (ImGui::Begin(label.data(), nullptr, flags)) {
                for (const auto& child : node.children()) {
                    ImGui::PushID(child.internal_object());
                    renderer.render(child, context);
                    ImGui::PopID();
                }
            }
            ImGui::End();

        } else {
            auto current = jsonutil::getBool(context, open, false);
            auto value = current;

            if (!current) return;

            if (ImGui::Begin(label.data(), &value, flags)) {
                for (const auto& child : node.children()) {
                    ImGui::PushID(child.internal_object());
                    renderer.render(child, context);
                    ImGui::PopID();
                }
            }
            ImGui::End();

            if (!value) {
                context[open] = value;
                renderer.markNeedsUpdate();
            }
        }

    }

    NEWUI_ELEMENT(Columns) {
        auto cols = jsonutil::getInt(node, "columns", 2);
        auto flags = jsonutil::getInt(node, "flags", 0);

        ImGui::PushID(&node);
        ImGui::BeginColumns("", cols, flags);
        for (const auto& child : node.children()) {
            ImGui::PushID(child.internal_object());
            renderer.render(child, context);
            ImGui::PopID();
            ImGui::NextColumn();
        }
        ImGui::EndColumns();
        ImGui::PopID();
    }

    NEWUI_ELEMENT(Row) {
        auto offset = jsonutil::getFloat(node, "offset", 0.0f);
        auto spacing = jsonutil::getFloat(node, "spacing", -1.0f);
        float itemSpacing = (spacing >= 0.0f) ? spacing : ImGui::GetStyle().ItemSpacing.x;

        size_t elementCount = 0;
        float totalWeight = 0.0f;
        bool hasZeroWeight = false;

        for (const auto& child : node.children()) {
            if (child.type() != pugi::node_element) continue;
            ++elementCount;
            auto w = jsonutil::getFloat(child, "weight", 0.0f);
            totalWeight += w;
            if (w <= 0.0f) hasZeroWeight = true;
        }

        if (elementCount == 0) return;

        if (totalWeight == 0.0f) {
            auto i = 0;
            ImGui::BeginGroup();
            for (const auto& child : node.children()) {
                if (child.type() != pugi::node_element) continue;
                ImGui::PushID(child.internal_object());
                if (i++ != 0) ImGui::SameLine(offset, spacing);
                renderer.render(child, context);
                ImGui::PopID();
            }
            ImGui::EndGroup();
            return;
        }

        if (hasZeroWeight) {
            ImGui::TextUnformatted("<% ERROR: Element in Row had weight of 0 %>");
        }

        float availWidth = ImGui::GetContentRegionAvail().x;
        float totalSpacing = itemSpacing * static_cast<float>(elementCount - 1);
        float usableWidth = std::max(0.0f, availWidth - totalSpacing - 0.0001f);

        auto i = 0;
        ImGui::BeginGroup();
        for (const auto& child : node.children()) {
            if (child.type() != pugi::node_element) continue;

            float childWeight = jsonutil::getFloat(child, "weight", 0.0f);
            float allocatedWidth = (totalWeight > 0.0f) ? (usableWidth * (childWeight / totalWeight)) : 0.0f;

            if (i++ != 0) ImGui::SameLine(offset, itemSpacing);
            ImGui::PushID(child.internal_object());
            if (ImGui::BeginChild(ImGui::GetID(child.internal_object()), ImVec2(allocatedWidth, 0.0f), ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
                ImGui::PushItemWidth(allocatedWidth);
                renderer.render(child, context);
                ImGui::PopItemWidth();
            }
            ImGui::EndChild();
            ImGui::PopID();
        }
        ImGui::EndGroup();
    }

    NEWUI_ELEMENT(Column) {
        auto userSpacing = jsonutil::getFloat(node, "spacing", -1.0f);
        float itemSpacing = (userSpacing >= 0.0f) ? userSpacing : ImGui::GetStyle().ItemSpacing.y;

        size_t elementCount = 0;
        float totalWeight = 0.0f;
        bool hasZeroWeight = false;

        for (const auto& child : node.children()) {
            if (child.type() != pugi::node_element) continue;
            ++elementCount;
            auto w = jsonutil::getFloat(child, "weight", 0.0f);
            totalWeight += w;
            if (w <= 0.0f) hasZeroWeight = true;
        }

        if (elementCount == 0) return;

        if (totalWeight == 0.0f) {
            for (const auto& child : node.children()) {
                if (child.type() != pugi::node_element) continue;
                ImGui::PushID(child.internal_object());
                renderer.render(child, context);
                ImGui::PopID();
            }
            return;
        }

        if (hasZeroWeight) {
            ImGui::TextUnformatted("<% ERROR: Element in Column had weight of 0 %>");
        }

        float availHeight = ImGui::GetContentRegionAvail().y;
        float totalSpacing = itemSpacing * static_cast<float>(elementCount - 1);
        float usableHeight = std::max(0.0f, availHeight - totalSpacing);

        auto i = 0;
        for (const auto& child : node.children()) {
            if (child.type() != pugi::node_element) continue;

            float childWeight = jsonutil::getFloat(child, "weight", 1.0f);
            float allocatedHeight = (totalWeight > 0.0f) ? (usableHeight * (childWeight / totalWeight)) : 0.0f;

            ImGui::PushID(child.internal_object());
            if (ImGui::BeginChild(ImGui::GetID(child.internal_object()), ImVec2(0, allocatedHeight), ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
                renderer.render(child, context);
            }
            ImGui::EndChild();
            ImGui::PopID();

            // if (++i < static_cast<int>(elementCount) && itemSpacing > 0.0f) {
            //     ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemSpacing);
            // }
        }
    }

    NEWUI_ELEMENT(Child) {
        auto id = jsonutil::getString(node, "id", node.path());
        auto size = jsonutil::getImVec2(node, "size", ImVec2(0, 0));
        auto flags = jsonutil::getInt(node, "flags", ImGuiChildFlags_NavFlattened);
        auto windowFlags = jsonutil::getInt(node, "windowflags", ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);
        if (ImGui::BeginChild(id.data(), size, flags, windowFlags)) {
            for (const auto& child : node.children()) {
                ImGui::PushID(child.internal_object());
                renderer.render(child, context);
                ImGui::PopID();
            }
        }
        ImGui::EndChild();
    }

    NEWUI_ELEMENT(CollapsingHeader) {
        auto label = jsonutil::getString(node, "label", "<% ERROR: Missing attribute 'label' %>");
        auto flags = jsonutil::getInt(node, "flags", 0);

        if (ImGui::CollapsingHeader(label.data(), flags)) {
            for (const auto& child : node.children()) {
                ImGui::PushID(child.internal_object());
                renderer.render(child, context);
                ImGui::PopID();
            }
        }
    }

    NEWUI_ELEMENT(Group) {
        ImGui::BeginGroup();
        for (const auto& child : node.children()) {
            ImGui::PushID(child.internal_object());
            renderer.render(child, context);
            ImGui::PopID();
        }
        ImGui::EndGroup();
    }
}
