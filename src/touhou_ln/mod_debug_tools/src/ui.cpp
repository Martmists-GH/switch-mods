#ifdef IMGUI_ENABLED
#include "ui/ui.h"

using namespace ui;


void setup_ui() {
    ROOT.Window([](Window& _) {
        _.title = STR(MODULE_NAME_SPACES) " - By Martmists";
        _.toggleable = false;
        // _.flags |= ImGuiWindowFlags_MenuBar;
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(400, 450);

        _.Text("Press ZL+R to toggle all menus.\nHold Y to move or resize.");
        _.Text("Early access on patreon.com/martmists");

        _.Spacing();
    });
}
#endif
