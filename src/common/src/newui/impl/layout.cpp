#include "imgui.h"
#include "imgui_internal.h"
#include "logger/logger.h"
#include "newui/newui.h"
#include "util/InputUtil.h"

namespace newui {
    NEWUI_ELEMENT(Root) {
        for (auto& child : node.children) {
            renderer.render(child, context);
        }
    }

    NEWUI_ELEMENT(Window) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto flags = node.get<int>("flags", ImGuiWindowFlags_NoCollapse);
        auto initialSize = node.get<ImVec2>("size", ImVec2(0, 0));
        auto initialPos = node.get<ImVec2>("position", ImVec2(0, 0));
        auto open = node.get<std::string_view>("open", "");
        auto sticky = node.get<bool>("sticky", false);

        if (!sticky && open.empty() && !InputUtil::isInputToggled()) return;

        if (initialPos.x != 0 || initialPos.y != 0) {
            ImGui::SetNextWindowPos(initialPos, ImGuiCond_FirstUseEver);
        }

        if (initialSize.x != 0 && initialSize.y != 0) {
            ImGui::SetNextWindowSize(initialSize, ImGuiCond_FirstUseEver);
        }

        if (open.empty()) {
            if (ImGui::Begin(label.data(), nullptr, flags)) {
                for (auto& child : node.children) {
                    renderer.render(child, context);
                }
            }
            ImGui::End();

        } else {
            auto current = jsonutil::getBool(context, open, false);
            auto value = current;

            if (!current) return;

            if (ImGui::Begin(label.data(), &value, flags)) {
                for (auto& child : node.children) {
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

    NEWUI_ELEMENT(Columns) {
        auto cols = node.get<int>("columns", 2);
        auto flags = node.get<int>("flags", 0);

        ImGui::PushID(&node);
        ImGui::BeginColumns("", cols, flags);
        for (auto& child : node.children) {
            renderer.render(child, context);
            ImGui::NextColumn();
        }
        ImGui::EndColumns();
        ImGui::PopID();
    }

    NEWUI_ELEMENT(Row) {
        auto offset = node.get<float>("offset", 0.0f);
        auto spacing = node.get<float>("spacing", -1.0f);
        float itemSpacing = (spacing >= 0.0f) ? spacing : ImGui::GetStyle().ItemSpacing.x;

        size_t elementCount = 0;
        float totalWeight = 0.0f;
        bool hasZeroWeight = false;

        for (auto& child : node.children) {
            ++elementCount;
            auto w = child.get<float>("weight", 0.0f);
            totalWeight += w;
            if (w <= 0.0f) hasZeroWeight = true;
        }

        if (elementCount == 0) return;

        if (totalWeight == 0.0f) {
            auto i = 0;
            ImGui::BeginGroup();
            for (auto& child : node.children) {
                if (i++ != 0) ImGui::SameLine(offset, spacing);
                renderer.render(child, context);
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
        for (auto& child : node.children) {
            float childWeight = child.get<float>("weight", 0.0f);
            float allocatedWidth = (totalWeight > 0.0f) ? (usableWidth * (childWeight / totalWeight)) : 0.0f;

            if (i++ != 0) ImGui::SameLine(offset, itemSpacing);
            if (ImGui::BeginChild(ImGui::GetID(child.node.internal_object()), ImVec2(allocatedWidth, 0.0f), ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
                ImGui::PushItemWidth(allocatedWidth);
                renderer.render(child, context);
                ImGui::PopItemWidth();
            }
            ImGui::EndChild();
        }
        ImGui::EndGroup();
    }

    NEWUI_ELEMENT(Column) {
        auto userSpacing = node.get<float>("spacing", -1.0f);
        float itemSpacing = (userSpacing >= 0.0f) ? userSpacing : ImGui::GetStyle().ItemSpacing.y;

        size_t elementCount = 0;
        float totalWeight = 0.0f;
        bool hasZeroWeight = false;

        for (auto& child : node.children) {
            ++elementCount;
            auto w = child.get<float>("weight", 0.0f);
            totalWeight += w;
            if (w <= 0.0f) hasZeroWeight = true;
        }

        if (elementCount == 0) return;

        if (totalWeight == 0.0f) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
            return;
        }

        if (hasZeroWeight) {
            ImGui::TextUnformatted("<% ERROR: Element in Column had weight of 0 %>");
        }

        float availHeight = ImGui::GetContentRegionAvail().y;
        float totalSpacing = itemSpacing * static_cast<float>(elementCount - 1);
        float usableHeight = std::max(0.0f, availHeight - totalSpacing);

        for (auto& child : node.children) {
            float childWeight = child.get<float>("weight", 0.0f);
            float allocatedHeight = (totalWeight > 0.0f) ? (usableHeight * (childWeight / totalWeight)) : 0.0f;

            if (ImGui::BeginChild(ImGui::GetID(child.node.internal_object()), ImVec2(0, allocatedHeight), ImGuiChildFlags_NavFlattened | ImGuiChildFlags_AutoResizeX, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
                renderer.render(child, context);
            }
            ImGui::EndChild();

            // if (++i < static_cast<int>(elementCount) && itemSpacing > 0.0f) {
            //     ImGui::SetCursorPosY(ImGui::GetCursorPosY() + itemSpacing);
            // }
        }
    }

    NEWUI_ELEMENT(Child) {
        auto id = node.get<std::string_view>("id", "");
        auto size = node.get<ImVec2>("size", ImVec2(0, 0));
        auto flags = node.get<int>("flags", ImGuiChildFlags_NavFlattened);
        auto windowFlags = node.get<int>("windowflags", ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground);

        bool isOpen;
        if (id.empty()) {
            isOpen = ImGui::BeginChild(ImGui::GetID(node.node.internal_object()), size, flags, windowFlags);
        } else {
            isOpen = ImGui::BeginChild(id.data(), size, flags, windowFlags);
        }

        if (isOpen) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
        }
        ImGui::EndChild();
    }

    NEWUI_ELEMENT(CollapsingHeader) {
        auto label = node.get<std::string_view>("label", "<% ERROR: Missing attribute 'label' %>");
        auto flags = node.get<int>("flags", 0);

        if (ImGui::CollapsingHeader(label.data(), flags)) {
            for (auto& child : node.children) {
                renderer.render(child, context);
            }
        }
    }

    NEWUI_ELEMENT(Group) {
        ImGui::BeginGroup();
        for (auto& child : node.children) {
            renderer.render(child, context);
        }
        ImGui::EndGroup();
    }
}
