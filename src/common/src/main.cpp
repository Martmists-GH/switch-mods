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

#include "imgui_backend/hooks.h"

static WebsocketClient& getLogWs() {
    static WebsocketClient logWs("ws://" LOGGER_IP ":" STR(LOGGER_PORT));
    return logWs;
}

HkTrampoline<void> MainInitHook = hk::hook::trampoline([]() -> void {
    Logger::addListener([](const char *message, size_t len) {
        hk::svc::OutputDebugString(message, len);
    });

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
            Logger::log("log open failed: %d\n", res.GetInnerValueForDebug());
            disableFileLogger = false;
            return;
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
#endif
    Logger::log(STR(MODULE_NAME) " Loaded!\n");

    nn::oe::DisplayVersion display_version{};
    nn::oe::GetDisplayVersion(&display_version);
    Logger::log("Detected version: %s\n", display_version.name);

    Logger::log("Base offset: %#010x\n", hk::ro::getMainModule()->range().start());
    switch (get_host()) {
        case HARDWARE:
            Logger::log("Detected as Nintendo Switch (console)\n");
            break;
        case RYUJINX:
            Logger::log("Detected as Ryujinx (emulator)\n");
            break;
        case YUZU:
            Logger::log("Detected as Yuzu (emulator)\n");
            break;
    }

    MainInitHook.orig();
});

struct frame {
    frame* prev;
    ulong ret_addr;
};

static constexpr const char* xreg[32] = { "x0","x1","x2","x3","x4","x5","x6","x7","x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","x18","x19","x20","x21","x22","x23","x24","x25","x26","x27","x28","x29","x30","sp" };
static constexpr const char* wreg[32] = { "w0","w1","w2","w3","w4","w5","w6","w7","w8","w9","w10","w11","w12","w13","w14","w15","w16","w17","w18","w19","w20","w21","w22","w23","w24","w25","w26","w27","w28","w29","w30","wsp" };

inline int32_t sign_extend(uint32_t v, int bits) {
    uint32_t m = 1u << (bits - 1);
    return (v ^ m) - m;
}

static std::string disasm(uint32_t insn) {
    if ((insn & 0x7C000000) == 0x94000000) {
        int32_t imm = sign_extend(insn & 0x03FFFFFF, 26) << 2;
        return std::format("bl {:+#x}", imm);
    }
    if ((insn & 0xFFFFFC1F) == 0xD61F0000) {
        return std::format("br {}", xreg[(insn >> 5) & 0x1F]);
    }
    if ((insn & 0xFFFFFC1F) == 0xD63F0000) {
        return std::format("blr {}", xreg[(insn >> 5) & 0x1F]);
    }
    if ((insn & 0x7F000000) == 0x34000000) {
        int32_t off = sign_extend((insn >> 5) & 0x7FFFF, 19) << 2;
        return std::format("cbz {}, {:+#x}", wreg[insn & 0x1F], off);
    }
    if ((insn & 0x7F000000) == 0x35000000) {
        int32_t off = sign_extend((insn >> 5) & 0x7FFFF, 19) << 2;
        return std::format("cbnz {}, {:+#x}", wreg[insn & 0x1F], off);
    }

    if ((insn & 0x3B000000) == 0x39000000) {
        uint32_t size = (insn >> 30) & 0x3;
        uint32_t imm = ((insn >> 10) & 0xFFF) << size;
        uint32_t rn = (insn >> 5) & 0x1F;
        uint32_t rt = insn & 0x1F;

        bool load = insn & (1u << 22);
        const char* m = load ? "ldr" : "str";
        const char* const* r = (size == 3) ? xreg : wreg;

        return std::format("{} {}, [{}, #{:#x}]", m, r[rt], xreg[rn], imm);
    }

    return std::format(".word {:#010x}", insn);
}

static void reportException(nn::os::UserExceptionInfo const* info, const char* funcLog, const char* fileLog, int code, const char* reasonLog, int resCode) {
    std::string body;

    DEBUG("ERR:Header");
    if (resCode == -1) {
        body = "Unexpected abort occurred!\n";
    } else {
        body = std::format(
            "Abort occurred at {} in file {}\n"
            "Error Code: {}, Reason: {}, Result: {:x}\n",
            funcLog,
            fileLog,
            code,
            reasonLog,
            resCode
        );
    }

    DEBUG("ERR:Frame pointer");
    frame* fp;
    if (info != nullptr) {
        fp = reinterpret_cast<frame*>(info->FP.x);
    } else {
        fp = (frame*)__builtin_frame_address(0);
    }

    DEBUG("ERR:Stack trace");
    if (fp == nullptr) {
        body += "Stack trace: [could not recover]\n";
    } else {
        body += "Stack trace:\n";
        if (info != nullptr) {
            auto ptr = info->PC.x;
            hk::ro::RoModule* module;
            for (int i = 0; i < hk::ro::getNumModules(); ++i) {
                auto mod = hk::ro::getModuleByIndex(i);
                if (mod->range().start() <= ptr && ptr < mod->range().end()) {
                    module = mod;
                }
            }
            if (module == nullptr) {
                body += std::format(" 0x{:016x} (unknown)\n", ptr);
            } else {
                auto start = module->range().start();
                auto nnModule = module->getNnModule();
                const Elf64_Sym* bestFit = nullptr;
                for (int j = 0; j < nnModule->m_Symbols; ++j) {
                    auto& sym = nnModule->m_pDynSym[j];
                    if (sym.st_value + start < ptr && ptr < sym.st_value + start + sym.st_size) {
                        bestFit = &sym;
                    }
                }
                if (bestFit != nullptr) {
                    auto offsetInSym = ptr - (bestFit->st_value + start);
                    auto symPtr = nnModule->m_pStrTab + bestFit->st_name;
                    auto symNameDemangled = demangle(symPtr);
                    body += std::format(" 0x{:016x} ({}:0x{:x}) => {}+0x{:x}\n", ptr, module->getModuleName(), ptr - start, symNameDemangled, offsetInSym);
                } else {
                    body += std::format(" 0x{:016x} ({}:0x{:x})\n", ptr, module->getModuleName(), ptr - start);
                }
                body += std::format("  {}\n", disasm(*reinterpret_cast<int*>(ptr)));
            }
        }

        do {
            auto ptr = fp->ret_addr - 4;
            if (ptr == static_cast<ulong>(-4)) break;
            hk::ro::RoModule* module;
            for (int i = 0; i < hk::ro::getNumModules(); ++i) {
                auto mod = hk::ro::getModuleByIndex(i);
                if (mod->range().start() <= ptr && ptr < mod->range().end()) {
                    module = mod;
                }
            }
            if (module == nullptr) {
                body += std::format(" 0x{:016x} (unknown)\n", ptr);
            } else {
                auto start = module->range().start();
                auto nnModule = module->getNnModule();
                const Elf64_Sym* bestFit = nullptr;
                for (int j = 0; j < nnModule->m_Symbols; ++j) {
                    auto& sym = nnModule->m_pDynSym[j];
                    if (sym.st_value + start < ptr && ptr < sym.st_value + start + sym.st_size) {
                        bestFit = &sym;
                    }
                }
                if (bestFit != nullptr) {
                    auto offsetInSym = ptr - (bestFit->st_value + start);
                    auto symPtr = nnModule->m_pStrTab + bestFit->st_name;
                    auto symNameDemangled = demangle(symPtr);
                    body += std::format(" 0x{:016x} ({}:0x{:x}) => {}+0x{:x}\n", ptr, module->getModuleName(), ptr - start, symNameDemangled, offsetInSym);
                } else {
                    body += std::format(" 0x{:016x} ({}:0x{:x})\n", ptr, module->getModuleName(), ptr - start);
                }
                body += std::format("  {}\n", disasm(*reinterpret_cast<int*>(ptr)));
            }
            fp = fp->prev;
        } while (fp != nullptr);
    }

    DEBUG("ERR:Modules");
    body += "Modules:\n";
    for (int i = 0; i < hk::ro::getNumModules(); ++i) {
        auto module = hk::ro::getModuleByIndex(i);
        body += std::format(
            " {0}: 0x{1:016x}-0x{2:016x}\n",
            module->getModuleName(),
            module->range().start(),
            module->range().end()
        );
    }

    DEBUG("ERR:Registers");
    if (info != nullptr) {
        body += "Registers:\n";
        for (int i = 0; i < 29; ++i) {
            body += std::format(
                " X{0:<2} 0x{1:016x} {1}\n",
                i,
                info->CpuRegisters[i].x
            );
        }
        body += std::format(" FP  0x{0:016x} {0}\n", info->FP.x);
        body += std::format(" LR  0x{0:016x} {0}\n", info->LR.x);
        body += std::format(" SP  0x{0:016x} {0}\n", info->SP.x);
        body += std::format(" PC  0x{0:016x} {0}\n", info->PC.x);
        if (info->PC.x != 0) {
            body += std::format("  {}", disasm(*reinterpret_cast<int*>(info->PC.x - 4)));
        }
    } else {
        body += "Registers: [unknown]\n";
    }

    // FIXME: Crashes on render thread
    Logger::log("!!!Abort!!!\n%s\n",  body.c_str());
    MessageUtil::popup(resCode, "!!!Abort!!!", body);
}

#ifdef __RTLD_PAST_13XX__
HkTrampoline<void, char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, char const*, std::va_list&> AbortHook = hk::hook::trampoline([](char const* cond, char const* func, char const* file, int code, nn::Result const* res, nn::os::UserExceptionInfo const* info, char const* fmt, std::va_list& args) {
#else
HkTrampoline<void, char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, nn::diag::MetaLogContext const&, char const*, std::va_list> AbortHook = hk::hook::trampoline([](char const* cond, char const* func, char const* file, int code, nn::Result const* res, nn::os::UserExceptionInfo const* info, nn::diag::MetaLogContext const& ctx, char const* fmt, std::va_list args) {
#endif
    const char* funcLog;
    const char* fileLog;
    const char* reasonLog;
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
    if (strlen(cond) == 0) {
        reasonLog = "[unknown reason]";
    } else {
        reasonLog = cond;
    }

    reportException(info, funcLog, fileLog, code, reasonLog, res->GetInnerValueForDebug());

#ifdef __RTLD_PAST_13XX__
    AbortHook.orig(cond, func, file, code, res, info, fmt, args);
#else
    AbortHook.orig(cond, func, file, code, res, info, ctx, fmt, args);
#endif
});

alignas(0x10) static char exceptionStack[0x4000];
static nn::os::UserExceptionInfo exceptionStorage;
void handleUserException(nn::os::UserExceptionInfo* info) {
    reportException(info, nullptr, nullptr, 0, nullptr, -1);
    nn::os::DefaultUserExceptionHandler(info);
}

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

    nn::os::SetUserExceptionHandler(&handleUserException, &exceptionStack, sizeof(exceptionStack), &exceptionStorage);
    nn::os::EnableUserExceptionHandlerOnDebugging(true);

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

#ifndef REMOVE_YUZU_WARNING
    show_yuzu_warning();
#endif
}
