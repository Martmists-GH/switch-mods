#ifdef IMGUI_ENABLED
#include "ui/ui.h"

#include <algorithm>
#include <format>
#include <externals/gfl/string.h>
#include <externals/ik/QuestManager.h>
#include <util/common_utils.h>

#include "externals/ik/event/IkkakuEventScriptCommand.h"
#include "externals/ik/EventSystemCallFunctions.h"
#include "externals/ik/FlagWorkManager.h"
#include "externals/ik/HudMomijiQuestAchievementUIAccessor.h"
#include "externals/ik/ResearchLevelManager.h"
#include "externals/ik/ZARoyaleSaveAccessor.h"
#include "externals/pe/text/lua/Text.h"

using namespace ui;

void setIsMustCapture(bool value);
void setIsExpShareOn(bool value);
void setExpMultiplier(int value);
void setExpMultiplierInvert(bool value);
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
            _.Checkbox("Invert Multipler (x2 -> x0.5)", [](bool value) {
                setExpMultiplierInvert(value);
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
                                Logger::log("Selecting: %s (%d)\n", entryNameC, strlen(entryNameC));
                                memcpy(&questSelectedNameBuffer, entryNameC, strlen(entryNameC));
                                questSelectedNameBuffer[strlen(entryNameC)] = 0;
                                Logger::log("Selected: %s\n", &questSelectedNameBuffer);
                                questSelected = &entry->value;
                            }
                        }

                        entry = entry->next;
                    }

                    ImGui::EndCombo();
                }
            });
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
