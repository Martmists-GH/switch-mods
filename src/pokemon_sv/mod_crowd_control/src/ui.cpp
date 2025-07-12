#include "ui/ui.h"

#include <cc_hooks.h>
#include <nn/os.h>

using namespace ui;

ELEMENT(EventSelector) {
    std::string defaultValue = "Select an event";

    std::vector<std::string> options = getEventList();
    std::string current = defaultValue;

    void draw() override {
        if (ImGui::BeginCombo("Event", current.c_str())) {
            for (int i = 0; i < options.size(); ++i) {
                bool is_selected = options[i] == current;
                if (ImGui::Selectable(options[i].c_str(), is_selected)) {
                    current = options[i];
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Execute event") && current != defaultValue) {
            triggerEvent(current);
        }
    }
};

void setup_ui() {
    ROOT.Window([](Window& _){
        _.title = "SV Crowd Control - By Martmists";
        _.toggleable = false;
        // _.flags |= ImGuiWindowFlags_MenuBar;
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(300, 500);

        // _.MenuBar([](MenuBar &_) {
        //
        // });

        _.Text([](Text &_) {
            _.content = "Press ZL+ZR to toggle all menus.";
        });

        _.Spacing();

        auto item = EventSelector::create([](EventSelector& _){});
        _.addChild(item);

        _.FunctionElement([](FunctionElement& _) {
            _.callback = [] {
                auto cfg = getConfig();

                ImGui::Separator();

                ImGui::Text("Number of options: %d", cfg.pollOptions);
                ImGui::Text("Poll duration: %d seconds", cfg.pollDuration);
                ImGui::Text("Poll interval: %d seconds", cfg.pollInterval);
                ImGui::Text("Effect duration: %d seconds", cfg.effectDuration);

                ImGui::Separator();

                auto flags = getFlags();

#define CC_INFO(name, key) if (flags.key##Active) { int64_t delta = flags.key##Timer - (nn::os::ConvertToTimeSpan(nn::os::GetSystemTick()).nanoseconds / (1000*1000*1000)); if (delta < 0) { ImGui::Text(name ": off*"); } else { ImGui::Text(name ": on - %ld seconds remaining", delta); } } else { ImGui::Text(name ": off"); }

                CC_INFO("1000x Exp gain", expGain1k);
                CC_INFO("No exp gain", expGainZero);
                CC_INFO("0%% Catch rate", neverCapture);
                CC_INFO("100%% Catch rate", alwaysCapture);
                CC_INFO("Invert type matchups", invertTypeEffects);
                CC_INFO("Random type matchups", randomTypeEffects);
                CC_INFO("No more type matchups", invertTypeEffects);
                CC_INFO("Apply a random filter", randomFilter);
                CC_INFO("STAB is bad", invertSTAB);
                CC_INFO("Disable ride abilities", disableRideAbilities);
                CC_INFO("Randomize wild pokemon", randomWildSpawns);
                CC_INFO("10x more wild pokemon", wildSpawnBoost);
                CC_INFO("Shiny season", increasedShinyOdds);
            };
        });
    });
}
