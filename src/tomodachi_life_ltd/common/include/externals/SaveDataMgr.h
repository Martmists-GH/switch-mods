#pragma once
#include "externals/util.h"
#include "nn/time_ClockSnapshot.h"
#include "sead/CriticalSection.h"
#include "sead/IDisposer.h"
#include "sead/Reflexible.h"
#include "sead/Singleton.h"
#include "sead/String.h"

struct astruct_13 : ExternalType<astruct_13> {
    struct vtable : sead::IDisposer::vtable {};
    struct fields : sead::IDisposer::fields {};
};

template <typename T>
struct SavedProperty : ExternalType<SavedProperty<T>> {
    struct vtable : sead::Reflexible::vtable {
        void* destructor;
        void* destructor2;
        void* getDataPtr;
        void* updateAndGetDataPtr;
        void* getValuePtr;
        void* getDataPtr2;
    };

    struct fields {
        T m_current;
        T m_old;
        int m_unk;
        int m_unk2;
        sead::CriticalSection::instance m_cs;
    };
};

struct astruct_32 {
    int value;
    bool isSet;
};

struct GameData_PlayerData {
    SavedProperty<astruct_32>::instance m_playerYear;
    SavedProperty<astruct_32>::instance m_playerMonth;
    SavedProperty<astruct_32>::instance m_playerDay;
    SavedProperty<long>::instance m_lastBirthdayTime;
    SavedProperty<sead::WFixedSafeString<0x20>>::instance m_playerName;
    SavedProperty<sead::WFixedSafeString<0x40>>::instance m_playerNamePronounce;
    SavedProperty<int>::instance m_playerNameRegion;
    SavedProperty<sead::WFixedSafeString<0x20>>::instance m_islandName;
    SavedProperty<sead::WFixedSafeString<0x40>>::instance m_islandNamePronounce;
    SavedProperty<int>::instance m_islandNameRegion;
    SavedProperty<nn::time::ClockSnapshot>::instance unk_ClockSnapshot;
    SavedProperty<int>::instance m_lastRegionLanguage;
    SavedProperty<long>::instance m_lastPenaltyTime;
    SavedProperty<int>::instance unk_a18_int_3;
    SavedProperty<int>::instance m_regionCode;
    SavedProperty<int>::instance m_region;
    SavedProperty<int>::instance m_hemisphere;
    SavedProperty<int>::instance m_currency;
    SavedProperty<int>::instance m_handColor;
    SavedProperty<int>::instance unk_a18_int_9;
    SavedProperty<int>::instance m_money;
    SavedProperty<int>::instance m_happiness;
    SavedProperty<int>::instance unk_a18_int_11;
    SavedProperty<int>::instance unk_a18_int_12;
    EXTERNAL_PAD(0x12090);
    EXTERNAL_PAD(0x14090);
    EXTERNAL_PAD(0x1cb0);
    EXTERNAL_PAD(0x177050);
    EXTERNAL_PAD(0x7d050);
    EXTERNAL_PAD(0x9690);
    EXTERNAL_PAD(0x4090);
    EXTERNAL_PAD(0x14090);
    EXTERNAL_PAD(0x4190);
    SavedProperty<int>::instance unk_a18_int_13;
    SavedProperty<long>::instance unk_a18_long_2;
    EXTERNAL_PAD(0x70);
    SavedProperty<bool>::instance m_foodShopDailyItem_isNeedShowNew;
    SavedProperty<bool>::instance m_foodShopDailyItem_isUnlockedOtherRegion;
    EXTERNAL_PAD(0x4b0);
    SavedProperty<bool>::instance m_whereWear_isNeedShowNew;
    SavedProperty<int>::instance m_roomStyleWeeklyItem_roomStyleVariationGroupStringId;
    SavedProperty<bool>::instance m_roomStyleWeeklyItem_isNeedShowNew;
    SavedProperty<int>::instance unk_a18_int_15;
    SavedProperty<bool>::instance m_quikBuild_isNeedShowNew;
    SavedProperty<bool>::instance m_unk3_isNeedShowNew;
    SavedProperty<long>::instance m_lastBazaarPurchasedTime;
    EXTERNAL_PAD(0x90);
    SavedProperty<long>::instance m_lastDonationGameTime;
    EXTERNAL_PAD(0x140);
    SavedProperty<int>::instance m_saveDataVersion;
    SavedProperty<long>::instance m_saveDataUniqueHash;
    SavedProperty<char>::instance unk_a18_char_5;
    SavedProperty<bool>::instance m_invertY;
    SavedProperty<char>::instance unk_a18_char_7;
    SavedProperty<char>::instance unk_a18_char_8;
    SavedProperty<char>::instance unk_a18_char_9;
    SavedProperty<char>::instance unk_a18_char_10;
    SavedProperty<char>::instance unk_a18_char_11;
    SavedProperty<int>::instance m_proposalFailureCount;
    SavedProperty<long>::instance unk_a18_long_6;
    SavedProperty<long>::instance m_lastIslandEditTime;
    SavedProperty<long>::instance m_specialSaleStartTime;
    SavedProperty<long>::instance m_specialSaleEndTime;
    SavedProperty<long>::instance m_firstBootTime;
    SavedProperty<int>::instance unk_a18_int_16;
    SavedProperty<int>::instance m_saveEventFlag;
    SavedProperty<long>::instance m_breakingNewsFlag;
    SavedProperty<long>::instance m_lastNewsWatchedTime;
    SavedProperty<int>::instance unk_a18_int_19;
    SavedProperty<long>::instance m_miiBirthdayNews_lastWatchedTime;
    SavedProperty<int>::instance m_miiBirthdayNews_watchedCount;
    EXTERNAL_PAD(0x2d0);
    SavedProperty<int>::instance m_volumeMusic;
    SavedProperty<int>::instance m_volumeSounds;
    SavedProperty<int>::instance m_volumeVoices;
    SavedProperty<long>::instance m_lastUpdateTime;
    SavedProperty<long>::instance m_lastEnterTime;
    EXTERNAL_PAD(0xb8);
    SavedProperty<char>::instance unk_a18_char_12;
    SavedProperty<char>::instance unk_a18_char_13;
};

static_assert(sizeof(GameData_PlayerData) == 0x243e10);

struct GameData_Player : ExternalType<GameData_Player> {
    struct vtable : astruct_13::vtable {

    };
    struct fields : astruct_13::fields {
        GameData_PlayerData m_data;
        EXTERNAL_PAD(8);
    };
};

struct GameData_Liberation : sead::Singleton<GameData_Liberation> {
    struct fields {
        EXTERNAL_PAD(0x18);
        SavedProperty<int>::instance m_fountainLevel;
        SavedProperty<int>::instance unk_a18_int_0;
        SavedProperty<int>::instance m_fountainWishes;
        SavedProperty<int>::instance unk_a18_int_1;
    };
};

enum SaveState : int {
    NONE = 0,
    SAVING = 1,
    SAVED = 2,
};

struct SaveDataMgr : sead::Singleton<SaveDataMgr> {
    struct fields {
        EXTERNAL_PAD(0x80);
        GameData_Player::instance* m_player;
        EXTERNAL_PAD(0x8bc);
        SaveState m_state;
        int m_version;
    };
};

static_assert(offsetof(SaveDataMgr::instance, fields.m_state) == 0x94c);
