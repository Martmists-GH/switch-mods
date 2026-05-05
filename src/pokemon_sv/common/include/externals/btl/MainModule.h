#pragma once

namespace btl {
    struct PokeIDTable {
        int* PokeIdToClientId(uint8_t pokeId);
    };

    struct PokeIDManager {
        EXTERNAL_PAD(0x50);
        PokeIDTable* pokeIdTable;
    };

    struct BattleEnvUtil {
        EXTERNAL_PAD(8);
        PokeIDManager* pokeIdManager;
    };

    struct BattleUnit;

    struct BTL_PARTY {
        btl::BattleUnit* members[6];
        uint8_t memberCount;
        EXTERNAL_PAD(7);
    };

    struct POKECON {
        EXTERNAL_PAD(0x28);
        btl::BTL_PARTY party[5];
    };

    struct BattleEnv {
        EXTERNAL_PAD(0x30);
        btl::POKECON* pokecon;
        EXTERNAL_PAD(0x258);
        BattleEnvUtil* battleEnvUtil;
    };

    static_assert(offsetof(BattleEnv, pokecon) == 0x30);
    static_assert(offsetof(BattleEnv, battleEnvUtil) == 0x290);

    struct MainModule {
        EXTERNAL_PAD(0x8a);
        short gameTime;
        EXTERNAL_PAD(0x9);
        uint8_t myClientID;
        EXTERNAL_PAD(0xba);
        BattleEnv* battleEnv;

        short GetGameLimitTime();
    };

    static_assert(offsetof(MainModule, myClientID) == 0x95);
    static_assert(offsetof(MainModule, battleEnv) == 0x150);
}
