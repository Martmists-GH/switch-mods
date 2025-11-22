#pragma once

namespace pml::pokepara {
    struct CalcData {
        uint8_t level;
        char unk;
        uint16_t maxHp;
        uint16_t atk;
        uint16_t def;
        uint16_t spd;
        uint16_t spatk;
        uint16_t spdef;
        int16_t hpOffset;
    };

    struct CoreDataBlockA {
        uint16_t monsno;
        uint16_t itemno;
        uint32_t id;
        uint32_t exp;
        uint16_t ability;
        short abilityFlag1 : 1;
        short abilityFlag2 : 1;
        short abilityFlag3 : 1;
        short unkFlag : 1;
        short pad : 12;
        short boxMarking;
        char pad2[2];
        uint32_t colorRnd;
        uint8_t nature;
        uint8_t natureMint;
        uint8_t fromEvent : 1;
        uint8_t sex : 2;
        uint8_t pad3 : 5;
        bool isOybn;
        uint16_t formno;
        uint8_t ev[6];
        char unk3[0x2c];
    };

    struct CoreDataBlockB {
        uint16_t nick[13];
        uint16_t waza[4];
        uint8_t wazaPP[4];
        uint8_t wazaPPUp[4];
        uint16_t eggWaza[4];
        uint16_t hp;
        uint32_t ivHp: 5;
        uint32_t ivAtk: 5;
        uint32_t ivDef: 5;
        uint32_t ivSpd: 5;
        uint32_t ivSpAtk: 5;
        uint32_t ivSpDef: 5;
        uint32_t egg: 1;
        uint32_t hasNick: 1;
        uint32_t sick;
        char unk[0x14];
    };
    struct CoreDataBlockC {
        char unk[0x50];
    };
    struct CoreDataBlockD {
        char unk[0x50];
    };

    union CoreDataBlock {
        CoreDataBlockA a;
        CoreDataBlockB b;
        CoreDataBlockC c;
        CoreDataBlockD d;
    };

    static_assert(sizeof(CoreDataBlockA) == 0x50);
    static_assert(sizeof(CoreDataBlockB) == 0x50);
    static_assert(sizeof(CoreDataBlockC) == 0x50);
    static_assert(sizeof(CoreDataBlockD) == 0x50);
    static_assert(sizeof(CoreDataBlock) == 0x50);

    struct SHUFFLE_ENTRY {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
    };

    static SHUFFLE_ENTRY SHUFFLE_TABLE[32] = {
        { 0, 1, 2, 3 },
        { 0, 1, 3, 2 },
        { 0, 2, 1, 3 },
        { 0, 3, 1, 2 },
        { 0, 2, 3, 1 },
        { 0, 3, 2, 1 },
        { 1, 0, 2, 3 },
        { 1, 0, 3, 2 },
        { 2, 0, 1, 3 },
        { 3, 0, 1, 2 },
        { 2, 0, 3, 1 },
        { 3, 0, 2, 1 },
        { 1, 2, 0, 3 },
        { 1, 3, 0, 2 },
        { 2, 1, 0, 3 },
        { 3, 1, 0, 2 },
        { 2, 3, 0, 1 },
        { 3, 2, 0, 1 },
        { 1, 2, 3, 0 },
        { 1, 3, 2, 0 },
        { 2, 1, 3, 0 },
        { 3, 1, 2, 0 },
        { 2, 3, 1, 0 },
        { 3, 2, 1, 0 },
        { 0, 1, 2, 3 },
        { 0, 1, 3, 2 },
        { 0, 2, 1, 3 },
        { 0, 3, 1, 2 },
        { 0, 2, 3, 1 },
        { 0, 3, 2, 1 },
        { 1, 0, 2, 3 },
        { 1, 0, 3, 2 },
    };

    struct CoreData {
        uint32_t personalRnd;
        short fastMode : 1;
        short unk : 1;
        short badEgg : 1;
        short pad : 13;
        uint16_t checksum;

        // Note: Accessing these requires the accessor being decoded
        CoreDataBlock blocks[4];

        CoreDataBlockA& GetCoreDataBlockA() {
            auto shuf = SHUFFLE_TABLE[(personalRnd >> 0xd) & 0x1f];
            return blocks[shuf.a].a;
        }

        CoreDataBlockB& GetCoreDataBlockB() {
            auto shuf = SHUFFLE_TABLE[(personalRnd >> 0xd) & 0x1f];
            return blocks[shuf.b].b;
        }

        CoreDataBlockC& GetCoreDataBlockC() {
            auto shuf = SHUFFLE_TABLE[(personalRnd >> 0xd) & 0x1f];
            return blocks[shuf.c].c;
        }

        CoreDataBlockD& GetCoreDataBlockD() {
            auto shuf = SHUFFLE_TABLE[(personalRnd >> 0xd) & 0x1f];
            return blocks[shuf.d].d;
        }
    };

    struct Accessor {
        void* unk;
        CalcData* calcData;
        CoreData* coreData;
        bool isEncoded;
        bool isFastMode;

        CoreDataBlockA& GetCoreDataBlockA() {
            return coreData->GetCoreDataBlockA();
        }

        CoreDataBlockB& GetCoreDataBlockB() {
            return coreData->GetCoreDataBlockB();
        }

        CoreDataBlockC& GetCoreDataBlockC() {
            return coreData->GetCoreDataBlockC();
        }

        CoreDataBlockD& GetCoreDataBlockD() {
            return coreData->GetCoreDataBlockD();
        }

        void StartFastMode();
        void EndFastMode();

        bool IsOybn();
        void SetOybn(bool value);
        uint8_t GetSex();
        void SetSex(uint8_t value);
        int GetLevel();
        void SetLevel(int value);
    };

    struct CoreParam : ExternalType<CoreParam> {
        struct vtable : gfl::ReferenceObject::vtable {};
        struct fields : gfl::ReferenceObject::fields {
            CoreData* m_coreData;
            char unk[0x38];
            Accessor* m_accessor;
        };

        bool IsRare();
        void UpdateCalcDatas(bool stayDead);
    };
};
