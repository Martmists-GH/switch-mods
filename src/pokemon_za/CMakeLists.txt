# Example Mod
create_mod(
    ZA_Example
    mod_example
    WITH_IMGUI
    WITH_HEAPSOURCE_DYNAMIC
    SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
    SDK_PAST_1900
    TITLE_ID "0100F43008C44000"
    GAME_TITLE "Pokemon Legends Z-A"
)

# Same Gender Rival mod
create_mod(
        ZA_Same_Gender_Rival
        mod_same_gender_rival
        WITH_IMGUI
        WITH_HEAPSOURCE_DYNAMIC
        SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
        SDK_PAST_1900
        TITLE_ID "0100F43008C44000"
        GAME_TITLE "Pokemon Legends Z-A"
)

# Debug Tools mod
create_mod(
        ZA_Debug_Tools
        mod_debug_tools
        WITH_IMGUI
        WITH_HEAPSOURCE_DYNAMIC
        SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
        SDK_PAST_1900
        TITLE_ID "0100F43008C44000"
        GAME_TITLE "Pokemon Legends Z-A"
)
