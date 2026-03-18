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

    struct BattleEnv {
        EXTERNAL_PAD(0x290);
        BattleEnvUtil* battleEnvUtil;
    };

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
