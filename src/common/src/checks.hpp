#pragma once

#include "nn/oe.h"
#include <string>
#include <cstring>
#include <hk/ro/RoUtil.h>
#include <hk/svc/api.h>
#include <logger/logger.h>

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

static NXHost get_host() {
    auto offset = hk::ro::getMainModule()->range().start();

    if ((offset & 0xffff0000) == 0x08500000) {
        return NXHost::RYUJINX;
    } else if (((offset == 0xffff0000) == 0x80000000) || ((offset == 0xffff0000) == 0x80080000)) {
        return NXHost::YUZU;
    } else {
        hk::Handle curProcess;
        hk::svc::getProcessHandleMesosphere(&curProcess);
        auto err = hk::svc::BreakDebugProcess(curProcess);
        if (!err.succeeded() && err.getValue() == ams::svc::ResultNotImplemented::InnerValue) {
            return NXHost::YUZU;
        } else {
            return NXHost::HARDWARE;
        }
    }
}

static bool is_emulator() {
    static NXHost host = get_host();
    return host != NXHost::HARDWARE;
}
