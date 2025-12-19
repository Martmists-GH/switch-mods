# Commissions
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/commission_vaulttecalex101")
    create_mod(
        SWSH_commission_vaulttecalex101
        commission_vaulttecalex101
        WITH_HEAPSOURCE_BSS
#        WITH_IMGUI
        TITLE_ID "0100abf008968000"
        GAME_TITLE "Pokemon Sword"
    )
else()
    message(WARNING "${CMAKE_CURRENT_SOURCE_DIR}/commission_vaulttecalex101 does not exist!")
endif()
