#ifdef IMGUI_ENABLED
#include "ui/ui.h"

#include <algorithm>
#include <format>
#include <externals/gfl/string.h>

#include "externals/ik/event/IkkakuEventScriptCommand.h"
#include "externals/ik/ZARoyaleSaveAccessor.h"
#include "externals/pe/text/lua/Text.h"

using namespace ui;

void setIsMustCapture(bool value);
void setIsExpShareOn(bool value);
void setExpMultiplier(int value);
void setIsMustShiny(bool value);

namespace ik::ItemId {
    extern char* names[2635];
}

void setup_ui() {
    ROOT.Window([](Window& _){
        _.title = "ZA Toolbox - By Martmists";
        _.toggleable = false;
        //_.flags |= ImGuiWindowFlags_MenuBar;
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(380, 500);

        _.MenuBar([](MenuBar &_) {

        });

        _.Text("Press ZL+R to toggle all menus.");

        _.Spacing();

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Money";
            _.Row([](Row &_) {
                _.Text("Money");
                _.FunctionElement([](FunctionElement &_) {
                    _.callback = [] {
                        auto numCurrency = ik::event::IkkakuEventScriptCommand::GetCurrentMoney();
                        ImGui::Text("%d", numCurrency);
                    };
                });
                _.Group([](Group &_) {
                    _.Button("+1000##Money", [] {
                        ik::event::IkkakuEventScriptCommand::AddAndShowMonyeUI(1000);
                    });
                    _.Button("-1000##Money", [] {
                        ik::event::IkkakuEventScriptCommand::AddAndShowMonyeUI(-1000);
                    });
                });
                _.Group([](Group &_) {
                    _.Button("+10000##Money", [] {
                        ik::event::IkkakuEventScriptCommand::AddAndShowMonyeUI(10000);
                    });
                    _.Button("-10000##Money", [] {
                        ik::event::IkkakuEventScriptCommand::AddAndShowMonyeUI(-10000);
                    });
                });
                _.Group([](Group &_) {
                    _.Button("+100000##Money", [] {
                        ik::event::IkkakuEventScriptCommand::AddAndShowMonyeUI(100000);
                    });
                    _.Button("-100000##Money", [] {
                        ik::event::IkkakuEventScriptCommand::AddAndShowMonyeUI(-100000);
                    });
                });
            });
            _.Row([](Row &_) {
                _.Text("Medals");
                _.FunctionElement([](FunctionElement &_) {
                    _.callback = [] {
                        auto numMedals = ik::ZARoyaleSaveAccessor::GetMedalNum();
                        ImGui::Text("%d", numMedals);
                    };
                });
                _.Group([](Group &_) {
                    _.Button("+100##Medals", [] {
                        auto newValue = ik::ZARoyaleSaveAccessor::GetMedalNum() + 100;
                        ik::ZARoyaleSaveAccessor::SetMedalNum(std::clamp(newValue, 0u, 9999u));
                    });
                    _.Button("-100##Medals", [] {
                        auto newValue = ik::ZARoyaleSaveAccessor::GetMedalNum() - 100;
                        ik::ZARoyaleSaveAccessor::SetMedalNum(std::clamp(newValue, 0u, 9999u));
                    });
                });
                _.Group([](Group &_) {
                    _.Button("+1000##Medals", [] {
                        auto newValue = ik::ZARoyaleSaveAccessor::GetMedalNum() + 1000;
                        ik::ZARoyaleSaveAccessor::SetMedalNum(std::clamp(newValue, 0u, 9999u));
                    });
                    _.Button("-1000##Medals", [] {
                        auto newValue = ik::ZARoyaleSaveAccessor::GetMedalNum() - 1000;
                        ik::ZARoyaleSaveAccessor::SetMedalNum(std::clamp(newValue, 0u, 9999u));
                    });
                });
            });
        });

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Battle";

            _.Checkbox("100% Capture Rate", false, [](bool it) {
                setIsMustCapture(it);
            });
            _.Checkbox("100% Shiny Rate", false, [](bool it) {
                setIsMustShiny(it);
            });
            _.Checkbox("Enable EXP Share", true, [](bool it) {
                setIsExpShareOn(it);
            });
            _.SliderInt([](SliderInt &_) {
                _.label = "EXP Multiplier";
                _.min = 0;
                _.max = 30;
                _.value = 1;
                _.onChange = [](int mult) {
                    setExpMultiplier(mult);
                };
            });
        });

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Bag";
            auto itemID = _.InputInt([](InputInt &_) {
                _.label = "Item ID";
                _.min = 1;
                _.value = 1;
                _.max = 2634;
            });
            _.FunctionElement([itemID]() {
                auto value = itemID->value;
                auto str = ik::ItemId::names[value];
                if (strlen(str) > 0) {
                    ImGui::Text("%s", str);
                } else {
                    ImGui::Text("[INVALID ITEM: %d]", value);
                }

                ImGui::SameLine();

                auto file = gfl::StringHolder::Create("itemname");
                auto label = std::vformat((value < 1000) ? "ITEMNAME_{:03d}" : "ITEMNAME_{:04d}", std::make_format_args(value));
                auto labelStr = gfl::StringHolder::Create(label.c_str());
                auto ptr = pe::text::lua::Text::GetText(&file, &labelStr);
                if (ptr.m_ptr == nullptr) {
                    ImGui::Text("[null]");
                } else {
                    auto text = ptr.m_ptr->asString();
                    ImGui::Text("%s", text.c_str());
                }
            });
            _.Row([itemID](Row &_) {
                _.Button("Add 1##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, 1);
                });
                _.Button("Add 10##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, 10);
                });
                _.Button("Add 100##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, 100);
                });
            });
        });
    });
}
#endif
