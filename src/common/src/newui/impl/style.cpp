#include "imgui.h"
#include "newui/newui.h"

namespace newui {
    enum VarType {
        Float,
        Vec2,
    };

    static struct {
        const char* key;
        ImGuiCol_ color;
    } colors[] = {
        {"text", ImGuiCol_Text},
        {"textdisabled", ImGuiCol_TextDisabled},
        {"windowbg", ImGuiCol_WindowBg},
        {"childbg", ImGuiCol_ChildBg},
        {"popupbg", ImGuiCol_PopupBg},
        {"border", ImGuiCol_Border},
        {"bordershadow", ImGuiCol_BorderShadow},
        {"framebg", ImGuiCol_FrameBg},
        {"framebghovered", ImGuiCol_FrameBgHovered},
        {"framebgactive", ImGuiCol_FrameBgActive},
        {"titlebg", ImGuiCol_TitleBg},
        {"titlebgactive", ImGuiCol_TitleBgActive},
        {"titlebgcollapsed", ImGuiCol_TitleBgCollapsed},
        {"menubarbg", ImGuiCol_MenuBarBg},
        {"scrollbarbg", ImGuiCol_ScrollbarBg},
        {"scrollbargrab", ImGuiCol_ScrollbarGrab},
        {"scrollbargrabhovered", ImGuiCol_ScrollbarGrabHovered},
        {"scrollbargrabactive", ImGuiCol_ScrollbarGrabActive},
        {"checkmark", ImGuiCol_CheckMark},
        {"slidergrab", ImGuiCol_SliderGrab},
        {"slidergrabactive", ImGuiCol_SliderGrabActive},
        {"button", ImGuiCol_Button},
        {"buttonhovered", ImGuiCol_ButtonHovered},
        {"buttonactive", ImGuiCol_ButtonActive},
        {"header", ImGuiCol_Header},
        {"headerhovered", ImGuiCol_HeaderHovered},
        {"headeractive", ImGuiCol_HeaderActive},
        {"separator", ImGuiCol_Separator},
        {"separatorhovered", ImGuiCol_SeparatorHovered},
        {"separatoractive", ImGuiCol_SeparatorActive},
        {"resizegrip", ImGuiCol_ResizeGrip},
        {"resizegriphovered", ImGuiCol_ResizeGripHovered},
        {"resizegripactive", ImGuiCol_ResizeGripActive},
        {"inputtextcursor", ImGuiCol_InputTextCursor},
        {"tabhovered", ImGuiCol_TabHovered},
        {"tab", ImGuiCol_Tab},
        {"tabselected", ImGuiCol_TabSelected},
        {"tabselectedoverline", ImGuiCol_TabSelectedOverline},
        {"tabdimmed", ImGuiCol_TabDimmed},
        {"tabdimmedselected", ImGuiCol_TabDimmedSelected},
        {"tabdimmedselectedoverline", ImGuiCol_TabDimmedSelectedOverline},
        {"plotlines", ImGuiCol_PlotLines},
        {"plotlineshovered", ImGuiCol_PlotLinesHovered},
        {"plothistogram", ImGuiCol_PlotHistogram},
        {"plothistogramhovered", ImGuiCol_PlotHistogramHovered},
        {"tableheaderbg", ImGuiCol_TableHeaderBg},
        {"tableborderstrong", ImGuiCol_TableBorderStrong},
        {"tableborderlight", ImGuiCol_TableBorderLight},
        {"tablerowbg", ImGuiCol_TableRowBg},
        {"tablerowbgalt", ImGuiCol_TableRowBgAlt},
        {"textlink", ImGuiCol_TextLink},
        {"textselectedbg", ImGuiCol_TextSelectedBg},
        {"treelines", ImGuiCol_TreeLines},
        {"dragdroptarget", ImGuiCol_DragDropTarget},
        {"dragdroptargetbg", ImGuiCol_DragDropTargetBg},
        {"unsavedmarker", ImGuiCol_UnsavedMarker},
        {"navcursor", ImGuiCol_NavCursor},
        {"navwindowinghighlight", ImGuiCol_NavWindowingHighlight},
        {"navwindowingdimbg", ImGuiCol_NavWindowingDimBg},
        {"modalwindowdimbg", ImGuiCol_ModalWindowDimBg},
    };

    static struct {
        const char* key;
        ImGuiStyleVar_ var;
        VarType type;
    } styleVars[] = {
        {"alpha", ImGuiStyleVar_Alpha, VarType::Float},
        {"disabledalpha", ImGuiStyleVar_DisabledAlpha, VarType::Float},
        {"windowpadding", ImGuiStyleVar_WindowPadding, VarType::Vec2},
        {"windowrounding", ImGuiStyleVar_WindowRounding, VarType::Float},
        {"windowbordersize", ImGuiStyleVar_WindowBorderSize, VarType::Float},
        {"windowminsize", ImGuiStyleVar_WindowMinSize, VarType::Vec2},
        {"windowtitlealign", ImGuiStyleVar_WindowTitleAlign, VarType::Vec2},
        {"childrounding", ImGuiStyleVar_ChildRounding, VarType::Float},
        {"childbordersize", ImGuiStyleVar_ChildBorderSize, VarType::Float},
        {"popuprounding", ImGuiStyleVar_PopupRounding, VarType::Float},
        {"popupbordersize", ImGuiStyleVar_PopupBorderSize, VarType::Float},
        {"framepadding", ImGuiStyleVar_FramePadding, VarType::Vec2},
        {"framerounding", ImGuiStyleVar_FrameRounding, VarType::Float},
        {"framebordersize", ImGuiStyleVar_FrameBorderSize, VarType::Float},
        {"itemspacing", ImGuiStyleVar_ItemSpacing, VarType::Vec2},
        {"iteminnerspacing", ImGuiStyleVar_ItemInnerSpacing, VarType::Vec2},
        {"indentspacing", ImGuiStyleVar_IndentSpacing, VarType::Float},
        {"cellpadding", ImGuiStyleVar_CellPadding, VarType::Vec2},
        {"scrollbarsize", ImGuiStyleVar_ScrollbarSize, VarType::Float},
        {"scrollbarrounding", ImGuiStyleVar_ScrollbarRounding, VarType::Float},
        {"scrollbarpadding", ImGuiStyleVar_ScrollbarPadding, VarType::Float},
        {"grabminsize", ImGuiStyleVar_GrabMinSize, VarType::Float},
        {"grabrounding", ImGuiStyleVar_GrabRounding, VarType::Float},
        {"imagebordersize", ImGuiStyleVar_ImageBorderSize, VarType::Float},
        {"tabrounding", ImGuiStyleVar_TabRounding, VarType::Float},
        {"tabbordersize", ImGuiStyleVar_TabBorderSize, VarType::Float},
        {"tabminwidthbase", ImGuiStyleVar_TabMinWidthBase, VarType::Float},
        {"tabminwidthshrink", ImGuiStyleVar_TabMinWidthShrink, VarType::Float},
        {"tabbarbordersize", ImGuiStyleVar_TabBarBorderSize, VarType::Float},
        {"tabbaroverlinesize", ImGuiStyleVar_TabBarOverlineSize, VarType::Float},
        {"tableangledheadersangle", ImGuiStyleVar_TableAngledHeadersAngle, VarType::Float},
        {"tableangledheaderstextalign", ImGuiStyleVar_TableAngledHeadersTextAlign, VarType::Vec2},
        {"treelinessize", ImGuiStyleVar_TreeLinesSize, VarType::Float},
        {"treelinesrounding", ImGuiStyleVar_TreeLinesRounding, VarType::Float},
        {"buttontextalign", ImGuiStyleVar_ButtonTextAlign, VarType::Vec2},
        {"selectabletextalign", ImGuiStyleVar_SelectableTextAlign, VarType::Vec2},
        {"separatortextbordersize", ImGuiStyleVar_SeparatorTextBorderSize, VarType::Float},
        {"separatortextalign", ImGuiStyleVar_SeparatorTextAlign, VarType::Vec2},
        {"separatortextpadding", ImGuiStyleVar_SeparatorTextPadding, VarType::Vec2},
    };

    NEWUI_ELEMENT(Style) {
        int colorPushes = 0;
        int varPushes = 0;

        for (auto& entry : colors) {
            if (!node.attribute(entry.key).empty()) {
                auto col = jsonutil::getImColor(node, entry.key, ImColor());
                ImGui::PushStyleColor(entry.color, col.Value);
                colorPushes++;
            }
        }

        for (auto& entry : styleVars) {
            if (!node.attribute(entry.key).empty()) {
                if (entry.type == VarType::Float) {
                    auto f = jsonutil::getFloat(node, entry.key, 0);
                    ImGui::PushStyleVar(entry.var, f);
                } else {
                    auto v = jsonutil::getImVec2(node, entry.key, ImVec2());
                    ImGui::PushStyleVar(entry.var, v);
                }
                varPushes++;
            }
        }

        for (const auto& child : node.children()) {
            renderer.render(child, context);
        }

        if (colorPushes > 0) ImGui::PopStyleColor(colorPushes);
        if (varPushes > 0) ImGui::PopStyleVar(varPushes);
    }
}
