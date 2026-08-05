#include "imgui.h"
#include "newui/newui.h"

namespace newui {
    NEWUI_ELEMENT(Include) {
        auto source = props.value("source", "");
        if (source.empty()) {
            ImGui::TextUnformatted("<% ERROR: Missing attribute 'source' %>");
        } else {
            if (!renderer.includeCache.contains(source)) {
                auto child = Renderer(&renderer);
                child.init(source.c_str());
                renderer.includeCache[source] = std::move(child);
            }

            renderer.includeCache[source].render(context);
        }
    }

    NEWUI_ELEMENT(Text) {
        auto content = props.value("content", "<% ERROR: Missing attribute 'content' %>");
        ImGui::TextUnformatted(content.c_str());
    }

    NEWUI_ELEMENT(SeparatorText) {
        auto content = props.value("content", "<% ERROR: Missing attribute 'content' %>");
        ImGui::SeparatorText(content.c_str());
    }

    NEWUI_ELEMENT(Spacing) {
        ImGui::Spacing();
    }
}
