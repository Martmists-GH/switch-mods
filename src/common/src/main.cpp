#include <checks.hpp>
#include <externals/util.h>
#include <hk/hook/Trampoline.h>
#include <hk/init/module.h>
#include <logger/logger.h>
#include <util/MessageUtil.h>
#include <net/WebsocketClient.h>
#include <nn/oe.h>
#include <nn/os_exception.h>
#include <nn/fs/fs_files.h>
#include <nn/fs/fs_mount.h>
#include <nn/fs_detail.h>
#include <util/FileUtil.h>
#include <util/ThreadUtil.h>
#include <nn/diag.h>
#include "game_constants.h"
#include "hk/os/Thread.h"

#ifdef HK_ADDON_HeapSourceBss
#include "hk/mem/BssHeap.h"
#endif
#include "imgui_backend/hooks.h"

static WebsocketClient& getLogWs() {
    static WebsocketClient logWs("ws://" LOGGER_IP ":" STR(LOGGER_PORT));
    return logWs;
}

static void handleWs(WebsocketClient* ws) {
    while (ws->open()) {
        ws->receive();
        nn::os::SleepThread(nn::TimeSpan::FromSeconds(3));
    }
}

HkTrampoline<void> MainInitHook = hk::hook::trampoline([]() -> void {
    Logger::addListener(hk::svc::OutputDebugString);

#ifndef DISABLE_LOGGER
    if (nn::fs::MountSdCardForDebug("sd")) {
        MessageUtil::abort(1, "Unable to mount SD card!", "An error occurred mounting the SD card!");
    }

    if (FileUtil::exists("sd:/mod.log")) {
        FileUtil::deleteFile("sd:/mod.log");
    }
    FileUtil::writeFile("sd:/mod.log", "[Start of logger output]\n");

    static bool disableFileLogger = false;
    Logger::addListener([](const char *message, size_t len) {
        if (disableFileLogger) return; // avoid recursion

        nn::fs::FileHandle file{};
        long size;
        if (auto res = nn::fs::OpenFile(&file, "sd:/mod.log", nn::fs::OpenMode_Write | nn::fs::OpenMode_Append); res.IsFailure()) {
            disableFileLogger = true;
            Logger::log("log open failed: %d", res.GetInnerValueForDebug());
            disableFileLogger = false;
            return;
        }
        nn::fs::GetFileSize(&size, file);

        nn::fs::WriteFile(file, size, message, len, nn::fs::WriteOption::CreateOption(0));
        nn::fs::WriteFile(file, size + len, "\n", 1, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        nn::fs::CloseFile(file);
    });

    // Handle ws PINGs
#ifdef HK_DEBUG
    static auto& ws = getLogWs();
    if (ws.open()) {
        Logger::addListener([](const char *message, size_t len) {
            ws.send(std::string(message, len));
        });

        // static to keep a reference to it
        static auto threadRef = hk::os::Thread(handleWs, &ws);
        threadRef.setName("Websocket Thread");
        threadRef.start();

        Logger::log("Connected to logging server!");
    } else {
        Logger::log("Failed to connect to logging server!");
    }
#endif
#endif
    Logger::log(STR(MODULE_NAME) " Loaded!\n");

    nn::oe::DisplayVersion display_version{};
    nn::oe::GetDisplayVersion(&display_version);
    Logger::log("Detected version: %s", display_version.name);

    Logger::log("Base offset: %#010x", hk::ro::getMainModule()->range().start());
    switch (get_host()) {
        case HARDWARE:
            Logger::log("Detected as Nintendo Switch (console)");
            break;
        case RYUJINX:
            Logger::log("Detected as Ryujinx (emulator)");
            break;
        case YUZU:
            Logger::log("Detected as Yuzu (emulator)");
            break;
    }

#ifdef HK_ADDON_HeapSourceBss
    Logger::log("Hakkun Heap Size: %x", HAKKUN_BSS_HEAP_SIZE);
#endif

    MainInitHook.orig();
});

#ifdef ENABLE_FS_LOG
static char buffer[0x200];
HkTrampoline<void, nn::Result, char const*> fsErrorResultLogHook = hk::hook::trampoline([](nn::Result r, char const* msg) {
    Logger::log("FS Access Error: %s (%d)", msg, r.GetInnerValueForDebug());
    fsErrorResultLogHook.orig(r, msg);
});

HkTrampoline<void, nn::Result, char const*> fsErrorLogHook = hk::hook::trampoline([](nn::Result r, char const* msg) {
    snprintf(buffer, sizeof(buffer) - 1, msg);
    buffer[0x200 - 1] = 0;
    Logger::log("FS Access Error: %s (%d)", buffer, r.GetInnerValueForDebug());
    fsErrorLogHook.orig(r, msg);
});

HkTrampoline<bool> fsEnableDebugLogHook = hk::hook::trampoline([]() {
    return true;
});

HkTrampoline<void, char const*> fsDebugLogHook = hk::hook::trampoline([](char const* msg) {
    snprintf(buffer, sizeof(buffer) - 1, msg);
    buffer[0x200 - 1] = 0;
    Logger::log("FS Access Debug: %s", buffer);
});
#endif

extern "C" extern void nnMain();

void error_init();
void mod_init();
extern "C" void hkMain() {
    // Do common init/hooks
    MainInitHook.installAtPtr(&nnMain);

    error_init();

#ifdef ENABLE_FS_LOG
    fsErrorResultLogHook.installAtPtr(&nn::fs::detail::LogResultErrorMessage);
    fsErrorLogHook.installAtPtr(&nn::fs::detail::LogErrorMessage);
    fsEnableDebugLogHook.installAtPtr(&nn::fs::detail::IsEnabledAccessLog);
    fsDebugLogHook.installAtPtr(&nn::fs::detail::OutputAccessLogToOnlySdCard);
#endif

    mod_init();
#ifdef IMGUI_ENABLED
    imgui_hooks();
#endif

#ifdef HK_ADDON_HeapSourceBss
    hk::mem::initializeMainHeap();
#endif

#ifndef REMOVE_YUZU_WARNING
    show_yuzu_warning();
#endif
}
