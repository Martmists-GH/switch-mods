# Example Mod
create_mod(
    SV_Example
    mod_example
    WITH_VARIANTS
    WITH_IMGUI
    WITH_HEAPSOURCE_DYNAMIC

    SOURCE_SHALLOW "${CMAKE_SOURCE_DIR}/lib/lua"
)

create_mod_variant(
    SV_Example
    Scarlet
    0100A3D008C5C000
    "Pokemon Scarlet"
)

create_mod_variant(
    SV_Example
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
    WITH_HEAPSOURCE_DYNAMIC

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
