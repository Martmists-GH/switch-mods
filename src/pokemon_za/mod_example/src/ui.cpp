#ifdef IMGUI_ENABLED
#include "ui/ui.h"

#include <format>

using namespace ui;

void setup_ui() {
    ROOT.Window([](Window& _){
        _.title = "ZA Toolbox - By Martmists";
        _.toggleable = false;
        //_.flags |= ImGuiWindowFlags_MenuBar;
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(380, 500);

        // _.MenuBar([](MenuBar &_) {
        //
        // });

        _.Text([](Text &_) {
            _.content = "Press ZL+R to toggle all menus.";
        });

        _.Spacing();
    });
}
#endif
