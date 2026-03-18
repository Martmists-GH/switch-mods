# Example Mod
create_mod(
    SV_Trinity_Bypass
    mod_trinity_bypass
    WITH_VARIANTS
    SDK_PAST_1300

    SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
)

create_mod_variant(
    SV_Trinity_Bypass
    Scarlet
    0100A3D008C5C000
    "Pokemon Scarlet"
)

create_mod_variant(
    SV_Trinity_Bypass
    Violet
    01008F6008C5E000
    "Pokemon Violet"
)

# Crowd Control Mod
create_mod(
    SV_CrowdControl
    mod_crowd_control
    WITH_VARIANTS
    WITH_IMGUI
    SDK_PAST_1300

    SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
)

create_mod_variant(
    SV_CrowdControl
    Scarlet
    0100A3D008C5C000
    "Pokemon Scarlet"
)

create_mod_variant(
    SV_CrowdControl
    Violet
    01008F6008C5E000
    "Pokemon Violet"
)


# Debug Menu Mod
create_mod(
    SV_Debug_Menu
    mod_debug_tools
    WITH_VARIANTS
    WITH_IMGUI
    SDK_PAST_1300

    SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
)

create_mod_variant(
    SV_Debug_Menu
    Scarlet
    0100A3D008C5C000
    "Pokemon Scarlet"
)

create_mod_variant(
    SV_Debug_Menu
    Violet
    01008F6008C5E000
    "Pokemon Violet"
)

# Commissions
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/commission_vaulttecalex101")
    create_mod(
            SV_commission_vaulttecalex101
            commission_vaulttecalex101
            WITH_VARIANTS
            WITH_IMGUI
            SDK_PAST_1300

            SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
    )

    create_mod_variant(
            SV_commission_vaulttecalex101
            Scarlet
            0100A3D008C5C000
            "Pokemon Scarlet"
    )

    create_mod_variant(
            SV_commission_vaulttecalex101
            Violet
            01008F6008C5E000
            "Pokemon Violet"
    )
else()
    message(WARNING "${CMAKE_CURRENT_SOURCE_DIR}/commission_vaulttecalex101 does not exist!")
endif()
