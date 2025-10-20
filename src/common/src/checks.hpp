#pragma once

#include "nn/oe.h"
#include <string>
#include <cstring>
#include <hk/ro/RoUtil.h>
#include <hk/svc/api.h>

inline bool is_game(u64 game_id) {
    u64 id;
    hk::svc::GetInfo(&id, hk::svc::InfoType::InfoType_ProgramId, 0xFFFF8001, 0);
    return id == game_id;
}

inline std::string get_version() {
    nn::oe::DisplayVersion display_version{};
    nn::oe::GetDisplayVersion(&display_version);
    return display_version.name;
}

inline bool is_version(const char* version) {
    nn::oe::DisplayVersion display_version{};
    nn::oe::GetDisplayVersion(&display_version);
    return strcmp(display_version.name, version) == 0;
}

inline bool is_game_version(u64 game_id, const char* version) {
    return is_game(game_id) && is_version(version);
}

enum NXHost {
    HARDWARE,
    RYUJINX,
    YUZU
};

inline NXHost get_host() {
    auto offset = hk::ro::getMainModule()->range().start();

    if (offset == 0x08504000 || offset == 0x08505000) {
        return NXHost::RYUJINX;
    } else if (offset == 0x80004000 || offset == 0x80005000) {
        return NXHost::YUZU;
    } else {
        return NXHost::HARDWARE;
    }
}

inline bool is_emulator() {
    return get_host() != NXHost::HARDWARE;
}
