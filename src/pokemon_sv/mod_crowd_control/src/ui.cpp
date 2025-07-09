#include "ui/ui.h"

using namespace ui;

void setup_ui() {
    ROOT.Window([](Window& _){
        _.title = "SV Toolbox - By Martmists";
        _.toggleable = false;
        _.flags |= ImGuiWindowFlags_MenuBar;
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(300, 500);

        _.MenuBar([](MenuBar &_) {

        });

        _.Text([](Text &_) {
            _.content = "Press ZL+ZR to toggle all menus.";
        });

        _.Spacing();

    });
}
