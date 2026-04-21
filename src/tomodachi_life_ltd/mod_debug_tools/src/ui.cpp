#ifdef IMGUI_ENABLED
#include "ui/ui.h"
#include <algorithm>
#include <codecvt>
#include <locale>

#include "externals/SaveDataMgr.h"

using namespace ui;

std::string utf16_to_utf8(const char16_t* utf16_str) {
    if (!utf16_str) return "";
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    return convert.to_bytes(utf16_str);
}

#define SDR (SaveDataMgr::s_instance->impl())
#define SDR_PLAYER (SDR->fields.m_player)
#define SDR_LIBERATION (GameData_Liberation::s_instance->impl())

void setup_ui() {
    ROOT.Window([](Window& _) {
        _.title = STR(MODULE_NAME_SPACES) " - By Martmists";
        _.toggleable = false;
#ifdef HK_DEBUG
        // _.sticky = true;
#endif
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(400, 450);

        _.Text("Press ZL+R to toggle all menus.\nHold Y to move or resize.");
        _.Text("Early access on patreon.com/martmists");

        _.Spacing();

#ifdef HK_DEBUG
        _.FunctionElement([]() {
            auto saveData = SaveDataMgr::s_instance;
            if (saveData) {
                auto& dat = SDR_PLAYER->fields.m_data;
                ImGui::Text("room: %d", dat.m_roomStyleWeeklyItem_roomStyleVariationGroupStringId.fields.m_current);
                ImGui::Text("A: %d", dat.m_roomStyleWeeklyItem_isNeedShowNew.fields.m_current);
                ImGui::Text("B: %d", dat.m_quikBuild_isNeedShowNew.fields.m_current);
                ImGui::Text("C: %d", dat.m_unk3_isNeedShowNew.fields.m_current);
                ImGui::Text("D: %d", dat.m_whereWear_isNeedShowNew.fields.m_current);
            }
        });
#endif

        _.FunctionElement([]() {
            if (SDR_PLAYER->fields.m_data.m_lastPenaltyTime.fields.m_current != 0) {
                ImGui::Text("A time penalty seems to be active!");
                ImGui::SameLine();
                if (ImGui::Button("Reset time penalty")) {
                    SDR_PLAYER->fields.m_data.m_lastPenaltyTime.fields.m_current = 0;
                }
            }
        });

        _.Grid([](Grid& _) {
            _.columns = 4;

            _.Text("Money");
            _.FunctionElement([]() {
                ImGui::Text("%.2f", SDR_PLAYER->fields.m_data.m_money.fields.m_current / 100.0f);
            });
            _.Group([](Group& _) {
                _.Button("Add 100##Money", []() {
                    SDR_PLAYER->fields.m_data.m_money.fields.m_current += 10000;
                });
                _.Button("Add 1000##Money", []() {
                    SDR_PLAYER->fields.m_data.m_money.fields.m_current += 100000;
                });
            });
            _.Group([](Group& _) {
                _.Button("Add 10000##Money", []() {
                    SDR_PLAYER->fields.m_data.m_money.fields.m_current += 1000000;
                });
                _.Button("Set to 0##Money", []() {
                    SDR_PLAYER->fields.m_data.m_money.fields.m_current = 0;
                });
            });

            _.Text("Fuzzies");
            _.FunctionElement([]() {
                ImGui::Text("%d", SDR_PLAYER->fields.m_data.m_happiness.fields.m_current);
            });
            _.Group([](Group& _) {
                _.Button("Add 100##Points", []() {
                    SDR_PLAYER->fields.m_data.m_happiness.fields.m_current += 100;
                });
                _.Button("Add 1000##Points", []() {
                    SDR_PLAYER->fields.m_data.m_happiness.fields.m_current += 1000;
                });
            });
            _.Group([](Group& _) {
                _.Button("Add 10000##Points", []() {
                    SDR_PLAYER->fields.m_data.m_happiness.fields.m_current += 10000;
                });
                _.Button("Set to 0##Points", []() {
                    SDR_PLAYER->fields.m_data.m_happiness.fields.m_current = 0;
                });
            });

            _.Text("Fountain Lv");
            _.FunctionElement([]() {
                ImGui::Text("%d", SDR_LIBERATION->fields.m_fountainLevel.fields.m_current);
            });
            _.Group([](Group& _) {
                _.Button("Add 1##Level", []() {
                    SDR_LIBERATION->fields.m_fountainLevel.fields.m_current += 1;
                });
                _.Button("Add 5##Level", []() {
                    SDR_LIBERATION->fields.m_fountainLevel.fields.m_current += 5;
                });
            });
            _.Group([](Group& _) {
                _.Button("Remove 1##Level", []() {
                    SDR_LIBERATION->fields.m_fountainLevel.fields.m_current -= 1;
                });
                _.Button("Remove 5##Level", []() {
                    SDR_LIBERATION->fields.m_fountainLevel.fields.m_current -= 5;
                });
            });

            _.Text("Wishes");
            _.FunctionElement([]() {
                ImGui::Text("%d", SDR_LIBERATION->fields.m_fountainWishes.fields.m_current);
            });
            _.Group([](Group& _) {
                _.Button("Add 1##Wishes", []() {
                    SDR_LIBERATION->fields.m_fountainWishes.fields.m_current += 1;
                });
                _.Button("Add 5##Wishes", []() {
                    SDR_LIBERATION->fields.m_fountainWishes.fields.m_current += 5;
                });
            });
            _.Group([](Group& _) {
                _.Button("Remove 1##Wishes", []() {
                    SDR_LIBERATION->fields.m_fountainWishes.fields.m_current -= 1;
                });
                _.Button("Remove 5##Wishes", []() {
                    SDR_LIBERATION->fields.m_fountainWishes.fields.m_current -= 5;
                });
            });
        });

        // _.CollapsingHeader([](CollapsingHeader &_) {
        //     _.label = "Money";
        //     _.FunctionElement([]() {
        //
        //     });
        // });
    });
}
#endif
