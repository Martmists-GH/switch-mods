#ifdef IMGUI_ENABLED
#include "ui/ui.h"

#include <algorithm>
#include <format>
#include <hook_util.h>
#include <map>
#include <numeric>
#include <externals/gfl/string.h>
#include <externals/ik/QuestManager.h>
#include <externals/pml/pokepara/CalcTool.h>
#include <util/common_utils.h>
#include "GlobalHeap_Utils.h"

#include "externals/cmn/savedata/BoxSaveDataAccessor.h"
#include "externals/cmn/GameData.h"
#include "externals/ik/event/IkkakuEventScriptCommand.h"
#include "externals/ik/BoxSaveUtil.h"
#include "externals/ik/EventSystemCallFunctions.h"
#include "externals/ik/FlagWorkManager.h"
#include "externals/ik/HudMomijiQuestAchievementUIAccessor.h"
#include "externals/ik/ResearchLevelManager.h"
#include "externals/ik/ZARoyaleSaveAccessor.h"
#include "externals/pe/text/lua/Text.h"
#include "externals/pml/personal/PersonalSystem.h"
#include "imgui_ext.h"

using namespace ui;

void setIsMustCapture(bool value);
void setIsExpShareOn(bool value);
void setExpMultiplier(int value);
void setExpMultiplierInvert(bool value);
void setAlwaysMaxEnergy(bool value);

namespace ik::ItemId {
    extern char* names[2635];
}

extern PokemonData s_dataForEncounter;

static const char* SEX_LIST[3] = {
    "Male",
    "Female",
    "Unknown",
};
static const char* STAT_KEY[6] = {
    "msg_ui_status_pokemon_hp_00",
    "msg_ui_status_pokemon_attack_00",
    "msg_ui_status_pokemon_defense_00",
    "msg_ui_status_pokemon_spdefense_00",
    "msg_ui_status_pokemon_spattack_00",
    "msg_ui_status_pokemon_speed_00",
};

template <typename T = ui::Builder>
void PokemonEditor(PokemonData* data, T& _, bool withExtraSettings) {
    _.Checkbox("Allow invalid forms", data->allowInvalidForms, [data](bool value) {
        data->allowInvalidForms = value;
    });
    _.TextSeparator("Pokemon");
    _.Grid([data](Grid &_) {
        _.columns = 2;
        auto monsno = _.InputInt([data](InputInt &_) {
            _.label = "Species ID";
            _.min = 1;
            _.value = data->species;
            _.max = 1010;
            _.onValueChanged = [data](int value){
                data->species = value;
            };
        });
        auto formno = _.InputInt([data](InputInt &_) {
            _.label = "Form ID";
            _.min = 0;
            _.value = data->form;
            _.max = 27;
            _.onValueChanged = [data](int value){
                data->form = value;
            };
        });
        _.FunctionElement([data, formno] {
            if (!data->allowInvalidForms) {
                if (!pml::personal::PersonalSystem::CheckPokeExist(data->species, data->form)) {
                    formno->value = 0;
                    formno->onValueChanged(0);
                }
            }
            if (!pml::personal::PersonalSystem::CheckPokeExist(data->species, data->form)) {
                ImGui::Text("[Invalid Pokemon]");
                ImGui::NextColumn();
                ImGui::Text("[Invalid Form]");
            } else {
                auto monsLabel = std::vformat((data->species < 1000) ? "MONSNAME_{:03d}" : "MONSNAME_{:04d}", std::make_format_args(data->species));
                auto monsPtr = pe::text::lua::Text::GetText("monsname", monsLabel.c_str());
                if (monsPtr.m_ptr == nullptr) {
                    ImGui::Text("[Invalid Pokemon]");
                } else {
                    ImGui::Text("%s", monsPtr.m_ptr->asString().c_str());
                }
                ImGui::NextColumn();

                auto formLabel = std::vformat((data->species < 1000) ? "ZKN_FORM_{:03d}_{:03d}" : "ZKN_FORM_{:04d}_{:03d}", std::make_format_args(data->species, data->form));
                auto formPtr = pe::text::lua::Text::GetText("zkn_form", formLabel.c_str());
                if (formPtr.m_ptr == nullptr) {
                    ImGui::Text("[Invalid Form]");
                } else {
                    ImGui::Text("%s", formPtr.m_ptr->asString().c_str());
                }
            }
        });
    });
    _.TextSeparator("Stats");
    _.Grid([data, withExtraSettings](Grid &_) {
        _.columns = 2;
        _.ComboSimple([data](ComboSimple &_) {
            _.label = "Sex";
            _.items = SEX_LIST;
            _.items_count = 3;
            _.selected = data->sex;
            _.onChange = [data](int index) {
                data->sex = index;
            };
        });
        _.FunctionElement([data] {
            static std::string items[25];
            static bool isInitialized = false;
            if (!isInitialized) {
                for (int i = 0; i < 25; i++) {
                    auto entry = std::format("SEIKAKU_{:03d}", i);
                    auto naturePtr = pe::text::lua::Text::GetText("seikaku", entry.c_str());
                    if (naturePtr.m_ptr == nullptr) {
                        items[i] = "[Invalid Nature]";
                    } else {
                        items[i] = naturePtr.m_ptr->asString();
                    }
                }
                isInitialized = true;
            }

            auto naturePtr = pe::text::lua::Text::GetText("box", "msg_ui_box_seikaku_01");
            auto natureStr = naturePtr.m_ptr->asString();
            if (ImGui::BeginCombo(natureStr.c_str(), items[data->nature].c_str(), ImGuiComboFlags_None)) {
                for (int i = 0; i < 25; i++) {
                    if (ImGui::Selectable(items[i].c_str(), data->nature == i)) {
                        data->nature = i;
                    }
                }
                ImGui::EndCombo();
            }
        });
        _.InputInt([data](InputInt &_) {
            _.label = "Level";
            _.min = 1;
            _.value = data->level;
            _.max = 100;
            _.onValueChanged = [data](int value) {
                data->level = value;
            };
        });
        _.InputInt([data](InputInt &_) {
            _.label = "Ability";
            _.min = 0;
            _.max = 2;
            _.value = data->ability;
            _.onValueChanged = [data](int value) {
                data->ability = value;
            };
        });
        if (withExtraSettings) {
            _.Checkbox("Shiny", data->forceShiny, [data](bool value) {
                data->forceShiny = value;
            });
            _.Checkbox("Alpha", data->forceAlpha, [data](bool value) {
                data->forceAlpha = value;
            });
        }
    });

    _.TextSeparator("IVs/EVs");
    _.Grid([data](Grid &_) {
        _.columns = 3;

        _.FunctionElement([](){});
        _.Text("IV");
        _.Text("EV");
        for (int i = 0; i < 6; i++) {
            _.FunctionElement([i]() {
                ImGui::TextFile("status", STAT_KEY[i]);
            });
            // _.Text(STAT[i]);
            _.InputInt([i, data](InputInt &_) {
                _.min = 0;
                _.value = data->iv[i];
                _.max = 31;
                _.label = "##IV" + std::to_string(i);
                _.onValueChanged = [i, data](int value) {
                    data->iv[i] = value;
                };
            });
            _.InputInt([i, data](InputInt &_) {
                _.min = 0;
                _.value = data->ev[i];
                _.max = 252;
                _.label = "##EV" + std::to_string(i);
                _.onValueChanged = [i, data](int value) {
                    data->ev[i] = value;
                };
            });
        }
    });
    _.FunctionElement([]() {
        ImGui::TextFile("status", "msg_ui_status_custom_title_00");
    });
    _.Grid([data](Grid &_) {
        _.columns = 4;
        for (int i = 0; i < 4; i++) {
            auto waza = _.InputInt([i, data](InputInt &_) {
                _.label = "##MOVES" + std::to_string(i);
                _.min = 0;
                _.value = data->moves[i];
                _.max = 920;
                _.onValueChanged = [i, data](int value) {
                    data->moves[i] = value;
                };
            });
            _.FunctionElement([waza]() {
                auto label = std::format("WAZANAME_{:03d}", waza->value);
                auto ptr = pe::text::lua::Text::GetText("wazaname", label.c_str());
                if (ptr.m_ptr == nullptr) {
                    ImGui::Text("[null]");
                } else {
                    auto text = ptr.m_ptr->asString();
                    ImGui::Text("%s", text.c_str());
                }
            });
        }
    });
}

static std::string memorySize(uint64_t numBytes) {
    if (numBytes < 1024) {
        return std::format("{} bytes", numBytes);
    } else if (numBytes < 1024 * 1024) {
        return std::format("{:.2f} kB", numBytes / 1024.0f);
    } else if (numBytes < 1024 * 1024 * 1024) {
        return std::format("{:.2f} MB", numBytes / 1024.0f / 1024.0f);
    } else {
        return std::format("{:.2f} GB", numBytes / 1024.0f / 1024.0f / 1024.0f);
    }
}

struct HeapMeta {
    uint64_t allocPeak;
    uint64_t lastAllocSize;
};

static bool ImguiHeapData(gfl::SizedHeap::instance* heap, HeapMeta &meta) {
    if (heap == nullptr) return false;
    ImGui::Text("%s", heap->fields.label);
    auto currentAlloc = heap->AllocSize();
    auto memAlloced = memorySize(currentAlloc);
    auto memAvailable = memorySize(heap->fields.allocMax);
    auto memPeak = memorySize(meta.allocPeak);
    auto delta = static_cast<long>(currentAlloc) - static_cast<long>(meta.lastAllocSize);
    auto deltaString = memorySize(std::abs(delta));
    ImGui::Text(" Usage: %s / %s", memAlloced.c_str(), memAvailable.c_str());
    ImGui::Text(" Peak: %s", memPeak.c_str());
    ImGui::Text(" Change: %s%s", delta < 0 ? "-" : "", deltaString.c_str());
    ImGui::Text(" # of objects: %ld", heap->AllocCount());

    meta.allocPeak = std::max(meta.allocPeak, currentAlloc);
    meta.lastAllocSize = currentAlloc;
    return true;
}

void setup_ui() {
    static auto perfWindow = ROOT.Window([](Window &_) {
        _.title = "Performance";
        _.sticky = true;
        _.open = false;
        _.toggleable = true;
        _.initialPos = ImVec2(800, 50);
        _.initialSize = ImVec2(450, 280);

        _.TextSeparator("FPS");

        static nn::TimeSpan lastTick = nn::os::ConvertToTimeSpan(nn::os::GetSystemTick());
        static std::array<float, 120> counts = {0};
        _.FunctionElement([]() {
            nn::TimeSpan curTick = nn::os::ConvertToTimeSpan(nn::os::GetSystemTick());
            float deltaTime = (curTick.nanoseconds - lastTick.nanoseconds) / 1e9f;
            lastTick = curTick;

            for (size_t i = 0; i < counts.size() - 1; ++i) {
                counts[i] = counts[i + 1];
            }
            counts.back() = deltaTime;

            float sum = 0.0f, minFT = FLT_MAX, maxFT = 0.0f;
            for (float t : counts) {
                sum += t;
                minFT = std::min(minFT, t);
                maxFT = std::max(maxFT, t);
            }

            float avgFT = sum / counts.size();
            float fps = 1.0f / avgFT;

            std::array<float, 120> sorted = counts;
            std::sort(sorted.begin(), sorted.end());
            auto pct = [&](float p) {
                float idx = p * (sorted.size() - 1);
                size_t i0 = idx;
                size_t i1 = std::min(i0 + 1, sorted.size() - 1);
                float t = idx - i0;
                return std::lerp(sorted[i0], sorted[i1], t);
            };

            float p50 = pct(0.50f);
            float p90 = pct(0.90f);
            float p99 = pct(0.99f);

            const size_t shortWindow = 30;
            float shortSum = 0.0f;
            for (size_t i = counts.size() - shortWindow; i < counts.size(); ++i) {
                shortSum += counts[i];
            }
            float shortAvg = shortSum / shortWindow;
            float shortFPS = 1.0f / shortAvg;

            ImGui::Text("FPS (120-frame avg): %.2f", fps);
            ImGui::Text("FPS (last 30): %.2f", shortFPS);
            ImGui::Text("Frame Time:\n avg %.3f ms\n min %.3f\n max %.3f", avgFT * 1000.0f, minFT * 1000.0f, maxFT * 1000.0f);
            ImGui::Text("Percentiles:\n P50 %.3f ms\n P90 %.3f ms\n P99 %.3f ms", p50 * 1000.0f, p90 * 1000.0f, p99 * 1000.0f);

            ImGui::PlotLines("Frame Times (ms)", counts.data(), counts.size(), 0,
                             nullptr, 0.0f, 0.2f);
        });

        _.TextSeparator("Memory");

        constexpr size_t HEAPMETA_MAX = 550;
        static std::array<HeapMeta, HEAPMETA_MAX> heapInfo = {};
        _.FunctionElement([]() {
            ImGui::BeginColumns(nullptr, 2, ImGuiOldColumnFlags_NoBorder);
            uint32_t j = 0;
            if (ImguiHeapData(gfl::SizedHeap::s_globalHeap, heapInfo[j++])) ImGui::NextColumn();
            for (uint32_t i = 0; i < gfl::HeapManager::s_managerCount; i++) {
                auto manager = (&gfl::HeapManager::s_managerList)[i];
                for (int k = 0; k < manager->GetCount(); k++) {
                    auto heap = manager->GetHeap(k);
                    if (j + 1 >= HEAPMETA_MAX) {
                        j++;
                    } else {
                        if (ImguiHeapData(heap, heapInfo[j++])) ImGui::NextColumn();
                    }
                }
            }
            ImGui::EndColumns();

            ImGui::Separator();
            auto stats = GetGlobalHeapState();
            ImGui::Text("GlobalHeap Wrapper: Allocated %d/%d objects", stats.first, stats.second);
            ImGui::Text("HeapMeta: Using %d/%d heaps", j, HEAPMETA_MAX);
        });
    });

    ROOT.Window([](Window& _){
        _.title = "ZA Toolbox - By Martmists";
        _.toggleable = false;
        _.flags |= ImGuiWindowFlags_MenuBar;
        _.initialPos = ImVec2(50, 50);
        _.initialSize = ImVec2(400, 450);

        _.FunctionElement([]() {
            static bool didLoad = false;
            if (!didLoad) {
                Logger::log("Loading message files\n");
                pe::text::lua::Text::LoadMsgData("ik_message/dat/JPN/common/seikaku.dat");
                pe::text::lua::Text::LoadMsgData("ik_message/dat/JPN/common/status.dat");
                pe::text::lua::Text::LoadMsgData("ik_message/dat/JPN/common/box.dat");
                pe::text::lua::Text::LoadMsgData("ik_message/dat/JPN/common/hud_itemget.dat");
                didLoad = true;
            }
        });

        _.MenuBar([](MenuBar &_) {
            _.Menu([](Menu &_) {
                _.label = "Windows";
                _.MenuItem([](MenuItem &_) {
                    _.label = "Performance";
                    _.checked = &perfWindow->open;
                });
            });
        });

        _.Text("Press ZL+R to toggle all menus.\nHold Y to move or resize.");
        _.Text("Early access on patreon.com/martmists");

        _.Spacing();

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Money";
            _.Grid([](Grid &_) {
                _.columns = 5;
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

                _.FunctionElement([]() {
                    ImGui::TextFile("hud_itemget", "hud_itemget_02_01");
                });
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
                _.Group([](Group &_) {
                    // Empty to match number of columns
                });

                // TODO: Tickets?
            });
        });

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Battle";

            _.Checkbox("100% Capture Rate", false, [](bool it) {
                setIsMustCapture(it);
            });
            _.Checkbox("Enable EXP Share", true, [](bool it) {
                setIsExpShareOn(it);
            });
            _.Row([](Row &_) {
                _.SliderInt([](SliderInt &_) {
                    _.label = "EXP Multiplier";
                    _.min = 0;
                    _.max = 30;
                    _.value = 1;
                    _.onChange = [](int mult) {
                        setExpMultiplier(mult);
                    };
                });
                _.Checkbox("Invert (x2 -> x0.5)", [](bool value) {
                    setExpMultiplierInvert(value);
                });
            });
            _.Checkbox("Always Max Mega Energy", [](bool it){
                setAlwaysMaxEnergy(it);
            });
        });

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Encounter Editor";
            _.Row([](Row &_){
                _.Checkbox("100% Shiny Rate", false, [](bool it) {
                    s_dataForEncounter.forceShiny = it;
                });
                _.SliderInt([](SliderInt &_) {
                    _.label = "Shiny rate Multiplier";
                    _.min = 0;
                    _.max = 60;
                    _.value = 1;
                    _.onChange = [](int mult) {
                        s_dataForEncounter.shinyMultiplier = mult;
                    };
                });
            });
            _.Checkbox("100% Alpha Rate", s_dataForEncounter.forceAlpha, [](bool it) {
                s_dataForEncounter.forceAlpha = it;
            });
            _.Separator();
            _.Checkbox("Force modify encounter", [](bool value) {
                s_dataForEncounter.forceModify = value;
            });

            _.FunctionElement([]() {
                auto instance = Builder::single();
                PokemonEditor(&s_dataForEncounter, instance, false);
                instance.render();
            });
        });

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Party";

            static bool enabled = false;
            static PokemonData s_partyData;
            auto partyIdx = _.InputInt([](InputInt &_) {
                _.min = 0;
                _.value = 0;
                _.max = 5;
                _.label = "Party Index";
            });
            _.Checkbox("Enable", false, [](bool it) {
                enabled = it;
            });
            _.FunctionElement([partyIdx]() {
                auto i = partyIdx->value;
                auto party = cmn::GameData::GetPlayerParty();
                auto param = party.m_ptr->GetMemberPtr(i);
                if (param.m_ptr == nullptr) {
                    return;
                }

                auto pp = param.m_ptr->fields.m_pp->castTo<pml::pokepara::CoreParam>();
                auto acc = pp->fields.m_accessor;

                // Ensure correct block order
                acc->StartFastMode();

                auto& a = acc->GetCoreDataBlockA();
                auto& b = acc->GetCoreDataBlockB();

                s_partyData.forceShiny = pp->IsRare();
                s_partyData.forceAlpha = a.isOybn;
                s_partyData.species = a.monsno;
                s_partyData.form = a.formNo;
                s_partyData.sex = acc->GetSex();
                s_partyData.ability = a.ability;
                s_partyData.nature = a.natureMint;
                s_partyData.level = acc->GetLevel();
                s_partyData.iv[0] = b.ivHp;
                s_partyData.iv[1] = b.ivAtk;
                s_partyData.iv[2] = b.ivDef;
                s_partyData.iv[3] = b.ivSpd;
                s_partyData.iv[4] = b.ivSpAtk;
                s_partyData.iv[5] = b.ivSpDef;
                for (int i = 0; i < 6; i++) {
                    s_partyData.ev[i] = a.ev[i];
                }
                for (int i = 0; i < 4; i++) {
                    s_partyData.moves[i] = b.waza[i];
                }

                auto builder = Builder::single();
                PokemonEditor(&s_partyData, builder, true);
                builder.render();

                if (!enabled) {
                    acc->EndFastMode();
                    return;
                };

                if (s_partyData.forceShiny) {
                    a.colorRnd = pml::pokepara::CalcTool::CorrectColorRndForRare(a.id, a.colorRnd);
                } else {
                    a.colorRnd = pml::pokepara::CalcTool::CorrectColorRndForNotRare(a.id, a.colorRnd);
                }
                a.isOybn = s_partyData.forceAlpha;
                a.monsno = s_partyData.species;
                a.formNo = s_partyData.form;
                acc->SetSex(s_partyData.sex);
                a.ability = s_partyData.ability;
                a.natureMint = s_partyData.nature;
                acc->SetLevel(s_partyData.level);
                b.ivHp = s_partyData.iv[0];
                b.ivAtk = s_partyData.iv[1];
                b.ivDef = s_partyData.iv[2];
                b.ivSpd = s_partyData.iv[3];
                b.ivSpAtk = s_partyData.iv[4];
                b.ivSpDef = s_partyData.iv[5];
                for (int i = 0; i < 6; i++) {
                    a.ev[i] = s_partyData.ev[i];
                }
                for (int i = 0; i < 4; i++) {
                    b.waza[i] = s_partyData.moves[i];
                }

                pp->UpdateCalcDatas(true);

                // Fix checksum to prevent bad egg
                acc->EndFastMode();
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

                auto label = std::vformat((value < 1000) ? "ITEMNAME_{:03d}" : "ITEMNAME_{:04d}", std::make_format_args(value));
                auto ptr = pe::text::lua::Text::GetText("itemname", label.c_str());
                if (ptr.m_ptr == nullptr) {
                    ImGui::Text("[null]");
                } else {
                    auto text = ptr.m_ptr->asString();
                    ImGui::Text("%s", text.c_str());
                }
            });
            // TODO: Remove items
            _.Grid([itemID](Grid &_) {
                _.columns = 3;
                _.Button("Add 1##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, 1);
                });
                _.Button("Add 10##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, 10);
                });
                _.Button("Add 100##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, 100);
                });
                _.Button("Remove 1##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, -1);
                });
                _.Button("Remove 10##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, -10);
                });
                _.Button("Remove 100##Items", [itemID]() {
                    ik::event::IkkakuEventScriptCommand::AddItem(itemID->value, -100);
                });
            });
        });

        static constexpr char* const QUEST_TYPES[3] = {
            "Main",
            "Side",
            "Lab",
        };
        static ik::quest::Quest* questSelected = nullptr;
        static char questSelectedNameBuffer[0x100] = {0};

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Quests";
            _.Text("Use at your own risk!");
            auto type = _.ComboSimple([](ComboSimple &_) {
                _.label = "Quest Type";
                _.items = QUEST_TYPES;
                _.items_count = 3;
                _.onChange = [](int arg) {
                    questSelected = nullptr;
                };
            });
            _.FunctionElement([type]() {
                auto& mgr = ik::QuestManager::s_instance;
                if (ImGui::BeginCombo("Quest", (questSelected == nullptr) ? nullptr : questSelectedNameBuffer)) {
                    ik::Quest* questContainer;
                    gfl::StringHolder filenameHolder;
                    switch (type->selected) {
                        default:
                            questContainer = mgr.m_mainQuests;
                            filenameHolder = gfl::StringHolder::Create("questlist_main");
                            break;
                        case 1:
                            questContainer = mgr.m_subQuests;
                            filenameHolder = gfl::StringHolder::Create("questlist_sub");
                            break;
                        case 2:
                            questContainer = mgr.m_labQuests;
                            filenameHolder = gfl::StringHolder::Create("questlist_mj");
                            break;
                    }
                    auto entry = questContainer->m_items.entries;
                    while (entry != nullptr) {
                        auto entryLabel = &entry->value.m_label->m_start;
                        auto labelHolder = gfl::StringHolder::Create(entryLabel);
                        auto entryNamePtr = pe::text::lua::Text::GetText(&filenameHolder, &labelHolder);
                        if (entryNamePtr.m_ptr != nullptr) {
                            auto entryName = entryNamePtr.m_ptr->asString();
                            auto entryNameC = entryName.c_str();
                            if (ImGui::Selectable(entryNameC, questSelected == &entry->value)) {
                                memcpy(&questSelectedNameBuffer, entryNameC, strlen(entryNameC));
                                questSelectedNameBuffer[strlen(entryNameC)] = 0;
                                questSelected = &entry->value;
                            }
                        }

                        entry = entry->next;
                    }

                    ImGui::EndCombo();
                }
            });
            _.Row([type](Row &_) {
                // FIXME: Not working :(
                // _.Button("Unlock Quest", [type]() {
                //     if (questSelected != nullptr) {
                //         auto workString = gfl::StringHolder::Create(&questSelected->m_work->m_start);
                //         ik::FlagWorkManager::s_instance.SetWorkValue(&workString, 1);
                //         ik::QuestManager::s_instance.SendQuestProgressUpdate(questSelected, type->selected);
                //     }
                // });
                _.Button("Complete Quest", [type]() {
                    if (questSelected != nullptr) {
                        auto holder = gfl::StringHolder::Create(&questSelected->m_id->m_start);

                        auto progress = questSelected->GetCurrentProgress();
                        if (progress->m_id == 255) return;

                        auto workString = gfl::StringHolder::Create(&questSelected->m_work->m_start);
                        while (true) {
                            ik::FlagWorkManager::s_instance.SetWorkValue(&workString, progress->m_nextId);
                            if (progress->m_nextId == 255) {
                                break;
                            }
                            progress = questSelected->GetCurrentProgress();
                        }

                        switch (type->selected) {
                            default: {
                                ik::QuestManager::s_instance.QuestComplete(&holder, false);
                                break;
                            }
                            case 1: {
                                ik::QuestManager::s_instance.QuestComplete(&holder, false);
                                break;
                            }
                            case 2: {
                                ik::QuestCompletion completion(ik::QuestManager::s_instance.m_resource, questSelected, type->selected);

                                completion.m_before.level = ik::ResearchLevelManager::s_instance.GetLevel();
                                completion.m_before.points = ik::ResearchLevelManager::s_instance.GetPoint();
                                completion.m_before.pointsNextLevel = ik::ResearchLevelManager::s_instance.GetNextLevelPoint();
                                completion.m_before.progress = ik::ResearchLevelManager::s_instance.GetNextLevelProgress();

                                ik::QuestManager::s_instance.QuestComplete(&holder, false);

                                if (ik::ResearchLevelManager::s_instance.CanLevelUp()) {
                                    ik::ResearchLevelManager::s_instance.ExecuteLevelUp();
                                }

                                completion.m_after.level = ik::ResearchLevelManager::s_instance.GetLevel();
                                completion.m_after.points = ik::ResearchLevelManager::s_instance.GetPoint();
                                completion.m_after.pointsNextLevel = ik::ResearchLevelManager::s_instance.GetNextLevelPoint();
                                completion.m_after.progress = ik::ResearchLevelManager::s_instance.GetNextLevelProgress();

                                ik::HudMomijiQuestAchievementUIAccessor::s_instance->AddCompletion(&completion);

                                break;
                            }
                        }

                        ik::QuestManager::s_instance.SendQuestProgressUpdate(questSelected, type->selected);
                    }
                });
            });
        });

        _.CollapsingHeader([](CollapsingHeader &_) {
            _.label = "Weather";
            _.Row([](Row &_) {
                _.Button("Sunny", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("sunny");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
                _.Button("Cloudy", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("cloudy");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
                _.Button("Rain", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("rain");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
            });
            _.Row([](Row &_) {
                _.Button("Storm", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("storm");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
                _.Button("Snow", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("snow");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
                _.Button("Snowstorm", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("snowstorm");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
            });
            _.Row([](Row &_) {
                _.Button("Diamonddust", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("diamonddust");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
                _.Button("Sandstorm", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("sandstorm");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
                _.Button("Mist", []() {
                    gfl::StringHolder holder = gfl::StringHolder::Create("mist");
                    gfa::EventContext ctx = gfa::EventContext::make(&holder);
                    ik::EventSystemCallFunctions::FixWeather(&ctx);
                });
            });
        });
    });
}
#endif
