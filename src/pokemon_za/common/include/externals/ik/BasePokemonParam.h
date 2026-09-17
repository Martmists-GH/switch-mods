#pragma once

#include "externals/gfl/object.h"
#include "externals/pml/pokepara/BasePokemonParam.h"

namespace ik {
    // IMPORTANT: This struct size seems to change across versions!
    struct UnitParam : ExternalType<UnitParam> {
        struct vtable : gfl::Object<>::vtable {
            void* GetTypehash;
            void* GetPower;
            void* GetMaxHp;
            void* GetHp;
            void* GetMaxHpOffset;
            void* SetHp;
            void* RecoverHp;
            void* IsHpZero;
            void* GetSick;
            void* unk;
            void* GetLevel;
            void* GetExp;
            void* SetExp;
            void* GetGiveExp;
            void* GetBasicPower;
            void* GetNativeTalentPower;
            void* GetEffortPower;
            void* ChangeEffortPower;
            void* GetWazaNo;
            void* GetWazaCount;
            void* SetWaza;
            void* unk2;
            void* unk3;
            void* unk4;
            void* unk5;
            void* unk6;
            void* GetTokuseiIndex;
            void* GetSeikaku;
            void* unk7;
            void* SetFriendship;
            void* GetFriendship;
            void* IsEgg;
            void* GetItemNo;
            void* SetItemNo;
            void* GetType1;
            void* GetType2;
            void* GetTypeEx;
            void* unk8;
            void* GetName;
            void* GetNickName;
            void* GetMonsNo;
            void* GetFormNo;
            void* Clone;
            void* StartFastMode;
            void* EndFastMode;
            void* GetMemories;
            void* SetMemories;
            void* GetMaxControllableLevel;
            void* SetMaxControllableLevel;
        };

        static_assert(sizeof(vtable) == 0x1f0);

        struct fields : gfl::Object<>::fields {
            void* m_Serializable;
            void* unk;
        };
    };

    struct BasePokemonParam : ExternalType<BasePokemonParam> {
        struct vtable : ik::UnitParam::vtable {
            void* unk1;
            void* unk2;
            void* unk3;
            void* unk4;
            void* unk5;
            void* unk6;
            void* CompareOwnerInfo;
            float (*GetRemainingWazaRecastTime)(BasePokemonParam*, int idx);
            float (*GetRemainingWazaRecastTimeMax)(BasePokemonParam*, int idx);
            void* unk7;
            void (*SetWazaRecastTime)(BasePokemonParam*, int idx, int type);
            // TODO: Finish
        };

        struct fields : ik::UnitParam::fields {
            pml::pokepara::PokemonParam* m_pp;
            EXTERNAL_PAD(4);
        };

        static_assert(offsetof(vtable, GetRemainingWazaRecastTime) == 0x228);
    };

    struct FieldPokemonParam : ExternalType<FieldPokemonParam> {
        struct vtable : ik::BasePokemonParam::vtable { };
        struct fields : ik::BasePokemonParam::fields {
            float m_wazaRecastTime[4];
            float m_wazaRecastTimeMax[4];
            EXTERNAL_PAD(12);
        };

        void SetWazaRecastTime(int waza, int type);
    };

    static_assert(offsetof(FieldPokemonParam::instance, fields.m_wazaRecastTime) == 0x5c);
}
