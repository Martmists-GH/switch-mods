#include <checks.hpp>
#include <hk/hook/Trampoline.h>
#include <hk/init/module.h>
#include <logger/logger.h>
#include <util/MessageUtil.h>
#include <net/WebsocketClient.h>
#include <nn/oe.h>
#include <nn/os.h>
#include <nn/fs/fs_files.h>
#include <nn/fs/fs_mount.h>
#include <nn/fs_detail.h>
#include <util/FileUtil.h>
#include <util/ThreadUtil.h>
#include <nn/diag.h>
#include "game_constants.h"

#include "imgui_backend/hooks.h"

static WebsocketClient& getLogWs() {
    static WebsocketClient logWs("ws://" LOGGER_IP ":" STR(LOGGER_PORT));
    return logWs;
}

HkTrampoline<void> MainInitHook = hk::hook::trampoline([]() -> void {
    Logger::addListener([](const char *message, size_t len) {
        hk::svc::OutputDebugString(message, len);
    });

    if (nn::fs::MountSdCardForDebug("sd")) {
        MessageUtil::abort(1, "Unable to mount SD card!", "An error occurred mounting the SD card!");
    }

    if (FileUtil::exists("sd:/mod.log")) {
        FileUtil::deleteFile("sd:/mod.log");
    }
    FileUtil::writeFile("sd:/mod.log", "");

    static bool fileHadError = false;
    Logger::addListener([](const char *message, size_t len) {
        if (fileHadError) return; // avoid recursion

        nn::fs::FileHandle file{};
        long size;
        if (auto res = nn::fs::OpenFile(&file, "sd:/mod.log", nn::fs::OpenMode_Write | nn::fs::OpenMode_Append); res.IsFailure()) {
            fileHadError = true;
            return Logger::log("log open failed: %d\n", res.GetInnerValueForDebug());
        }
        nn::fs::GetFileSize(&size, file);

        nn::fs::WriteFile(file, size, message, len, nn::fs::WriteOption::CreateOption(nn::fs::WriteOptionFlag_Flush));
        nn::fs::CloseFile(file);
    });

    // Handle ws PINGs
#ifdef HK_DEBUG
    auto& ws = getLogWs();
    if (ws.open()) {
        Logger::addListener([&ws](const char *message, size_t len) {
            ws.send(std::string(message, len));
        });

        // static to keep a reference to it
        static auto threadRef = ThreadUtil::createThread([&ws]() {
            while (ws.open()) {
                ws.receive();
                nn::os::SleepThread(nn::TimeSpan::FromSeconds(3));
            }
        });

        threadRef->start();
        Logger::log("Connected to logging server!\n");
    } else {
        Logger::log("Failed to connect to logging server!\n");
    }
#endif
    Logger::log(STR(MODULE_NAME) " Loaded!\n");

    nn::oe::DisplayVersion display_version{};
    nn::oe::GetDisplayVersion(&display_version);
    Logger::log("Detected version: %s\n", display_version.name);

    Logger::log("Base offset: %#010x\n", hk::ro::getMainModule()->range().start());
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

    MainInitHook.orig();
});

HkTrampoline<void, char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, char const*, std::va_list&> AbortHook = hk::hook::trampoline([](char const* cond, char const* func, char const* file, int code, nn::Result const* res, nn::os::UserExceptionInfo const* info, char const* fmt, std::va_list& args) {
    const char* funcLog;
    const char* fileLog;
    if (strlen(func) == 0) {
        funcLog = "[unknown function]";  // TODO: Print offset from module?
    } else {
        funcLog = func;
    }
    if (strlen(file) == 0) {
        fileLog = "[unknown file]";
    } else {
        fileLog = file;
    }
    Logger::log("!!!Abort!!!\nAbort occurred at %s in file %s\n", funcLog, fileLog);
    if (strlen(fmt) > 0) {
        Logger::log(fmt, args);
    }
    AbortHook.orig(cond, func, file, code, res, info, fmt, args);
});

#ifdef ENABLE_FS_LOG

static char buffer[0x200];
HkTrampoline<void, nn::Result, char const*> fsErrorResultLogHook = hk::hook::trampoline([](nn::Result r, char const* msg) {
    Logger::log("FS Access Error: %s (%d)\n", msg, r.GetInnerValueForDebug());
    fsErrorResultLogHook.orig(r, msg);
});

HkTrampoline<void, nn::Result, char const*> fsErrorLogHook = hk::hook::trampoline([](nn::Result r, char const* msg) {
    snprintf(buffer, sizeof(buffer) - 1, msg);
    buffer[0x200 - 1] = 0;
    Logger::log("FS Access Error: %s (%d)\n", buffer, r.GetInnerValueForDebug());
    fsErrorLogHook.orig(r, msg);
});

HkTrampoline<bool> fsEnableDebugLogHook = hk::hook::trampoline([]() {
    return true;
});

HkTrampoline<void, char const*> fsDebugLogHook = hk::hook::trampoline([](char const* msg) {
    // va_list args;
    // va_start(args, msg);
    // char buffer[0x200];
    // vsnprintf(buffer, sizeof(buffer) - 1, msg, args);
    snprintf(buffer, sizeof(buffer) - 1, msg);
    buffer[0x200 - 1] = 0;
    Logger::log("FS Access Debug: %s", buffer);
    // va_end(args);
});

#endif

extern "C" extern void nnMain();

void mod_init();
extern "C" void hkMain() {
    // Do common init/hooks
    MainInitHook.installAtPtr(&nnMain);
    AbortHook.installAtPtr(&nn::diag::detail::VAbortImpl);

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
}
