#include <imgui.h>
#include <externals/pe/text/lua/Text.h>

namespace ImGui {
    static void TextFile(const char* file, const char* key) {
        auto ref = pe::text::lua::Text::GetText(file, key);
        auto str = ref.m_ptr->asString();
        ImGui::Text("%s", str.c_str());
    }
}
