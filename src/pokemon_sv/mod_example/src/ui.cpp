#include "ui/ui.h"

#include <format>
#include <lua_utils.h>
#include "data/monsno.h"

using namespace ui;

static int species = 1;
static int form = 0;
static int level = 1;
static bool sex = false;
static int teraType = 0;
static int nature = 0;

enum stat {
    HP,
    ATK,
    DEF,
    SPATK,
    SPDEF,
    SPD
};

static int ivs[6] = {0};
static int evs[6] = {0};
static int move[4] = {0};

static std::string getPokeParamCode() {
    return std::format(R"(
local spec = cC99399C3.f101D811F()
spec:f2247607A({0})
spec:f81738FA1({1})
spec:f02983AD3({2})
if {3} then
    spec:f88C47BAC(1)
else
    spec:f88C47BAC(0)
end
local param = c113335A8.f7AC1FE6B(spec)
param:fB762BF0A({4})
-- TODO: Nature
-- TODO: Moves
-- TODO: EVs/IVs
)", species, form, level, sex, teraType, nature);
}

static std::string getPartyInsertCode(int index) {
    return std::format(R"(
local party = c1A1CBE3B.fFCE06E04()
party:f36E12C71({0}, param)
)", index);
}

void setup_ui() {
    ROOT.Window([](Window& _){
        _.title = "SV Toolbox - By Martmists";
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

#if 0
        _.CollapsingHeader([](CollapsingHeader&_) {
            _.label = "Utilities";

            _.Button([](Button&_) {
                _.label = "PrintIDs";
                _.onClick = [] {
                    executeLua(get_lua_state(), "API:logIDs()", "<UI:PrintIDs>")
                };
            });
        });
#endif

        _.CollapsingHeader([](CollapsingHeader& _) {
            _.label = "Pokemon editor";

            _.FunctionElement([](FunctionElement&_) {
                _.callback = []() {
                    // Species Combo
                    // Form Combo
                    // Level Input
                    // Tera Type
                    // Ability Combo
                    // Nature Combo
                    // Male/Female Toggle
                    // ---
                    // Moves
                    // Move 1 | Move 2
                    // Move 3 | Move 4
                    // ---
                    // IVs/EVs
                    // -- | IV | EV
                    // HP | XX | YY
                    // AT | XX | YY
                    // DF | XX | YY
                    // SA | XX | YY
                    // SD | XX | YY
                    // SP | XX | YY

                    // ======

                    // Species Combo
                    static int speciesIdx = 0;
                    if (ImGui::Combo("Species", &speciesIdx, MONSNO_NAMES, MONSNO_COUNT)) {
                        species = MONSNO_IDS[speciesIdx];
                        form = 0;
                    }

                    // Form Combo
                    auto& form_values = getFORMS()[speciesIdx];
                    auto form_text = std::vector<std::string>();
                    for (int fv : form_values) {
                        form_text.emplace_back(std::to_string(fv));
                    }
                    if (ImGui::BeginCombo("Form", form_text[form].c_str())) {
                        for (int i = 0; i < form_values.size(); i++) {
                            if (ImGui::Selectable(form_text[i].c_str(), form == form_values[i])) {
                                form = form_values[i];
                            }
                        }
                        ImGui::EndCombo();
                    }

                    // Level Input
                    if (ImGui::InputInt("Level", &level, 1, 10)) {
                        if (level < 1) {
                            level = 1;
                        } else if (level > 100) {
                            level = 100;
                        }
                    }

                    // Tera Type
                    ImGui::Combo("Tera Type", &teraType, TYPE_NAMES, TYPE_COUNT);

                    // Ability Combo

                    // Nature Combo
                    ImGui::Combo("Nature", &nature, NATURE_NAMES, NATURE_COUNT);

                    // Male/Female Toggle
                    ImGui::Checkbox("Female", &sex);

                    // ---
                    ImGui::Spacing();

                    // Moves
                    // Move 1 | Move 2
                    // Move 3 | Move 4
                    ImGui::Text("Moves");
                    if (ImGui::BeginTable("Moves", 2)) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Move 1 Dropdown");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("Move 2 Dropdown");
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Move 3 Dropdown");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("Move 4 Dropdown");

                        ImGui::EndTable();
                    }

                    // ---
                    // IVs/EVs
                    // -- | IV | EV
                    // HP | XX | YY
                    // AT | XX | YY
                    // DF | XX | YY
                    // SA | XX | YY
                    // SD | XX | YY
                    // SP | XX | YY
                };
            });

            _.Spacing();

            _.Button([](Button& _) {
                _.label = "Spawn";
                _.onClick = []() {
                    static std::string SPAWN_CODE = R"(
_G.idx = _G.idx or 0
local parent = C4EE52E49562F8277.SBF5E0EFFB821427F:f462C9B70()
local objName = "custom_spawn_" .. tostring(idx)
_G.idx = _G.idx + 1
local x, y, z = C4EE52E49562F8277.SBF5E0EFFB821427F:f7360ED03()
c05424CF6.fF104BCD4(parent, objName, param, 0, x, y, z, true)
                    )";

                    auto snippet = getPokeParamCode() + SPAWN_CODE;
                    executeLua(get_lua_state(), snippet.c_str(), "<UI:Spawn>");
                };
            });

            _.Row([](Row& _) {
                _.Button([](Button& _) {
                    _.label = "Set team #1";
                    _.onClick = []() {
                        auto snippet = getPokeParamCode() + getPartyInsertCode(0);
                        executeLua(get_lua_state(), snippet.c_str(), "<UI:Set Team 1>");
                    };
                });
                _.Button([](Button& _) {
                    _.label = "Set team #2";
                    _.onClick = []() {
                        auto snippet = getPokeParamCode() + getPartyInsertCode(1);
                        executeLua(get_lua_state(), snippet.c_str(), "<UI:Set Team 2>");
                    };
                });
                _.Button([](Button& _) {
                    _.label = "Set team #3";
                    _.onClick = []() {
                        auto snippet = getPokeParamCode() + getPartyInsertCode(2);
                        executeLua(get_lua_state(), snippet.c_str(), "<UI:Set Team 3>");
                    };
                });
            });
            _.Row([](Row& _) {
                _.Button([](Button& _) {
                    _.label = "Set team #4";
                    _.onClick = []() {
                        auto snippet = getPokeParamCode() + getPartyInsertCode(3);
                        executeLua(get_lua_state(), snippet.c_str(), "<UI:Set Team 4>");
                    };
                });
                _.Button([](Button& _) {
                    _.label = "Set team #5";
                    _.onClick = []() {
                        auto snippet = getPokeParamCode() + getPartyInsertCode(4);
                        executeLua(get_lua_state(), snippet.c_str(), "<UI:Set Team 5>");
                    };
                });
                _.Button([](Button& _) {
                    _.label = "Set team #6";
                    _.onClick = []() {
                        auto snippet = getPokeParamCode() + getPartyInsertCode(5);
                        executeLua(get_lua_state(), snippet.c_str(), "<UI:Set Team 6>");
                    };
                });
            });
        });
    });
}
