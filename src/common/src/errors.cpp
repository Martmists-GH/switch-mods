#include <checks.hpp>
#include <externals/util.h>
#include <hk/hook/Trampoline.h>
#include <hk/init/module.h>
#include <logger/logger.h>
#include <util/MessageUtil.h>
#include <net/WebsocketClient.h>
#include <nn/oe.h>
#include <nn/os_exception.h>
#include <util/FileUtil.h>
#include <util/ThreadUtil.h>
#include <nn/diag.h>

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

static std::pair<std::optional<std::string>, ptrdiff_t> functionAt(uintptr_t ptr) {
    hk::ro::RoModule* module = nullptr;
    for (int i = 0; i < hk::ro::getNumModules(); ++i) {
        auto mod = hk::ro::getModuleByIndex(i);
        if (mod->range().start() <= ptr && ptr < mod->range().end()) {
            module = mod;
        }
    }

    if (module == nullptr) {
        return std::make_pair(std::nullopt, -1);
    }

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
        return std::make_pair(symNameDemangled, offsetInSym);
    }

    return std::make_pair(std::nullopt, -1);
}

static bool isAscii(const char* ptr) {
    if (!ptr) return false;

    size_t length = 0;

    while (length < 256) {
        auto c = ptr[length];

        if (c == '\0') {
            return length >= 4;
        }

        if ((c >= 0x20 && c <= 0x7E) || c == '\r' || c == '\n' || c == '\t') {
            length++;
        } else {
            return false;
        }
    }

    return true;  // We never print more than 256 chars anyway
}

static std::string disasm(uint32_t* insnPtr) {
    auto insn = *insnPtr;

#ifdef NO_MALLOC_EXCEPTIONS
    return ".unknown";
#endif

    // BL
    if ((insn & 0xFC000000) == 0x94000000) {
        int32_t imm = sign_extend(insn & 0x03FFFFFF, 26) << 2;
        auto [targetFuncName, targetOffset] = functionAt((uintptr_t)insnPtr);

        if (targetFuncName.has_value()) {
            if (targetOffset == 0) {
                return std::format("bl {}", targetFuncName.value());
            } else {
                return std::format("bl {}+{}", targetFuncName.value(), targetOffset);
            }
        }
        return std::format("bl {:+#x}", ((uintptr_t)insnPtr) + imm);
    }

    // BR
    if ((insn & 0xFFFFFC1F) == 0xD63F0000) {
        return std::format("blr {}", xreg[(insn >> 5) & 0x1F]);
    }

    // LDR/STR unsigned offset
    if ((insn & 0x3B000000) == 0x39000000) {
        uint32_t size = (insn >> 30) & 0x3;
        bool vr = (insn >> 26) & 1;
        bool load = (insn >> 22) & 1;
        uint32_t imm = ((insn >> 10) & 0xFFF) << size;
        uint32_t rn = (insn >> 5) & 0x1F;
        uint32_t rt = insn & 0x1F;

        static const char* suffix[] = {"b","h","","",""};
        auto op = load ? "ldr" : "str";

        if (vr) {
            auto fr = (size == 3) ? "d" : "s";
            return std::format("{}{} {}{}, [{}, #{:#x}]",
                op, (size <= 1) ? suffix[size] : "",
                fr, rt, xreg[rn], imm);
        }

        auto r = (size == 3) ? xreg : wreg;
        if (size <= 1)
            return std::format("{}{} {}, [{}, #{:#x}]",
                op, suffix[size], wreg[rt], xreg[rn], imm);
        return std::format("{} {}, [{}, #{:#x}]",
            op, r[rt], xreg[rn], imm);
    }

    // LDR/STR pre-indexed ([Xn, #imm]!)
    if ((insn & 0x3B200C00) == 0x38000C00) {
        uint32_t size = (insn >> 30) & 0x3;
        bool load = (insn >> 22) & 1;
        int32_t imm = sign_extend((insn >> 12) & 0x1FF, 9);
        uint32_t rn = (insn >> 5) & 0x1F;
        uint32_t rt = insn & 0x1F;
        auto r = (size == 3) ? xreg : wreg;
        auto op = load ? "ldr" : "str";
        return std::format("{} {}, [{}, #{:+}]!", op, r[rt], xreg[rn], imm);
    }

    // LDR/STR post-indexed ([Xn], #imm)
    if ((insn & 0x3B200C00) == 0x38000400) {
        uint32_t size = (insn >> 30) & 0x3;
        bool load = (insn >> 22) & 1;
        int32_t imm = sign_extend((insn >> 12) & 0x1FF, 9);
        uint32_t rn = (insn >> 5) & 0x1F;
        uint32_t rt = insn & 0x1F;
        auto r = (size == 3) ? xreg : wreg;
        auto op = load ? "ldr" : "str";
        return std::format("{} {}, [{}], #{:+}", op, r[rt], xreg[rn], imm);
    }

    // LDR/STR register offset ([Xn, Xm, {LSL #0|#size}])
    if ((insn & 0x3B200C00) == 0x38200800) {
        uint32_t size = (insn >> 30) & 0x3;
        bool load = (insn >> 22) & 1;
        uint32_t rm = (insn >> 16) & 0x1F;
        uint32_t ext = (insn >> 13) & 0x7;
        bool s = (insn >> 12) & 1;
        uint32_t rn = (insn >> 5) & 0x1F;
        uint32_t rt = insn & 0x1F;
        auto r = (size == 3) ? xreg : wreg;
        auto op = load ? "ldr" : "str";
        bool rm64 = (ext == 3 || ext >= 6);
        static const char* extname[] = {"uxtb","uxth","uxtw","lsl","sxtb","sxth","sxtw","sxtx"};
        if (s && size > 0)
            return std::format("{} {}, [{}, {}, {} #{}]",
                op, r[rt], xreg[rn], rm64 ? xreg[rm] : wreg[rm],
                extname[ext], size);
        return std::format("{} {}, [{}, {}{}]",
            op, r[rt], xreg[rn], rm64 ? xreg[rm] : wreg[rm],
            (ext != 3) ? std::format(", {}", extname[ext]) : "");
    }

    // LDP/STP
    if (
        (insn & 0x7FC00000) == 0x29000000 ||
        (insn & 0x7FC00000) == 0x69000000 ||
        (insn & 0x7FC00000) == 0xA9000000 ||
        (insn & 0x7FC00000) == 0x6DC00000
    ) {
        uint32_t opc = (insn >> 30) & 0x3;
        bool vr = (insn >> 26) & 1;
        bool load = (insn >> 22) & 1;
        int32_t imm7 = sign_extend((insn >> 15) & 0x7F, 7);
        uint32_t rt2 = (insn >> 10) & 0x1F;
        uint32_t rn = (insn >> 5) & 0x1F;
        uint32_t rt = insn & 0x1F;
        int32_t imm = imm7 << (2 + (opc >> 1));
        auto r = (opc & 2) ? xreg : wreg;
        auto op = load ? "ldp" : "stp";
        if (vr) {
            auto fr = (opc & 2) ? "d" : "s";
            return std::format("{} {}{}, {}{}, [{}, #{:+}]",
                op, fr, rt, fr, rt2, xreg[rn], imm);
        }
        return std::format("{} {}, {}, [{}, #{:+}]",
            op, r[rt], r[rt2], xreg[rn], imm);
    }

    return std::format(".word {:#010x}", insn);
}

template <int bufferSize>
class FixedBuffer : public std::streambuf {
public:
    FixedBuffer() : std::streambuf() {
        std::memset(buffer, 0, sizeof(buffer));
        setp(buffer, &buffer[bufferSize-1]);         // Remember the -1 to preserve the terminator.
        setg(buffer, buffer, &buffer[bufferSize-1]); // Technically not necessary for an std::ostream.
    }

    std::string get() const
    {
        return buffer;
    }

private:
    char buffer[bufferSize];
};

#define REPORT_APPEND(format, ...) \
    memset(tmp, 0, sizeof(tmp)); \
    snprintf(tmp, sizeof(tmp), format, ##__VA_ARGS__); \
    os << tmp;

__attribute__((weak))
void onExceptionHandler() { }

static struct { uint32_t code; const char* msg; } errorMessages[] = {
    { 0x0100, "Invalid instruction access" },
    { 0x0101, "Invalid data access" },
    { 0x0102, "Unaligned instruction access" },
    { 0x0103, "Unaligned data access" },
    { 0x0104, "Undefined instruction" },
    { 0x0105, "Exceptional instruction" },
    { 0x0106, "Memory system error" },
    { 0x0200, "Floating point exception" },
    { 0x0301, "Invalid system call" },
};

static void reportException(nn::os::UserExceptionInfo const* info, const char* funcLog, const char* fileLog, int code, const char* reasonLog, int resCode) {
    onExceptionHandler();

    Logger::log("HANDLING EXCEPTION");

    FixedBuffer<0x2000> exceptionBuffer;
    char tmp[0x2000];
    std::ostream os(&exceptionBuffer);

    auto thread = nn::os::GetCurrentThread();

    if (resCode == -1) {
        REPORT_APPEND("Unexpected abort occurred in Thread %lu (%s)!\n", nn::os::GetThreadId(thread), nn::os::GetThreadNamePointer(thread));
    } else {
        REPORT_APPEND("Abort occurred at %s in file %s\n"
            "Result: %x [%04x-%04x]\n"
            "Thread: %lu (%s)\n"
            "Error Code: %d, Reason: %s\n",
            funcLog,
            fileLog,
            resCode,
            reinterpret_cast<nn::Result&>(resCode).GetModule() | 0x2000,
            reinterpret_cast<nn::Result&>(resCode).GetDescription(),
            nn::os::GetThreadId(thread),
            thread->_namePointer,
            code,
            reasonLog
        );
    }

    frame* fp;
    if (info != nullptr) {
        for (auto [e, m] : errorMessages) {
            if (info->ErrorDescription == e) {
                REPORT_APPEND("Error description: %s\n", m);
                if (e == 0x0301) {
                    REPORT_APPEND("System call: %d\n", info->ESR & 0x1ffffff);
                }

                break;
            }
        }

        fp = reinterpret_cast<frame*>(info->FP.x);
    } else {
        fp = (frame*)__builtin_frame_address(0);
    }

    if (fp == nullptr) {
        os << "Stack trace: [could not recover]\n";
    } else {
        os << "Stack trace:\n";
        if (info != nullptr) {
            u64 ptrs[] = {
                info->PC.x,
                info->LR.x - 4,
            };
            for (auto ptr : ptrs) {
                hk::ro::RoModule* module;
                for (int i = 0; i < hk::ro::getNumModules(); ++i) {
                    auto mod = hk::ro::getModuleByIndex(i);
                    if (mod->range().start() <= ptr && ptr < mod->range().end()) {
                        module = mod;
                    }
                }
                if (module == nullptr) {
                    REPORT_APPEND(" 0x%016lx (unknown)\n", ptr);
                } else {
                    auto [name, offset] = functionAt(ptr);
                    auto start = module->range().start();
                    if (name.has_value()) {
                        auto start = ptr - offset;
                        REPORT_APPEND(" 0x%016lx (%s:0x%lx) => %s+0x%lx\n", ptr, module->getModuleName(), ptr - start, name.value().c_str(), offset);
                    } else {
                        REPORT_APPEND(" 0x%016lx (%s:0x%lx)\n", ptr, module->getModuleName(), ptr - start)
                    }

                    auto s = disasm(reinterpret_cast<uint32_t*>(ptr));
                    REPORT_APPEND("  %s\n", s.c_str());
                }
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
                    if (i == 0) goto loop_end;  // Don't log nnrtld
                }
            }
            if (module == nullptr) {
                REPORT_APPEND(" 0x%016lx (unknown)\n", ptr);
            } else {
                auto [name, offset] = functionAt(ptr);
                auto start = module->range().start();
                if (name.has_value()) {
                    auto start = ptr - offset;
                    REPORT_APPEND(" 0x%016lx (%s:0x%lx) => %s+0x%lx\n", ptr, module->getModuleName(), ptr - start, name.value().c_str(), offset);
                } else {
                    REPORT_APPEND(" 0x%016lx (%s:0x%lx)\n", ptr, module->getModuleName(), ptr - start)
                }

                auto s = disasm(reinterpret_cast<uint32_t*>(ptr));
                REPORT_APPEND("  %s\n", s.c_str());
            }
            fp = fp->prev;
        } while (fp != nullptr);
        REPORT_APPEND("\n");

        loop_end:
    }

    os << "Modules:\n";
    for (int i = 0; i < hk::ro::getNumModules(); ++i) {
        auto module = hk::ro::getModuleByIndex(i);
        REPORT_APPEND(" %s: 0x%016lx-0x%016lx\n", module->getModuleName(), module->range().start(), module->range().end());
    }

    if (info != nullptr) {
        os << "Registers:\n";
        for (int i = 0; i < 29; ++i) {
            auto reg = info->CpuRegisters[i].x;
            auto found = false;
            for (int m = 0; m < hk::ro::getNumModules() && !found; ++m) {
                auto mod = hk::ro::getModuleByIndex(m);
                if (mod->range().start() <= reg && reg < mod->range().end()) {
                    // Check ptr for:
                    // - Functions
                    // - Strings
                    auto [name, offset] = functionAt(reg);
                    if (name.has_value()) {
                        if (offset == 0) {
                            REPORT_APPEND(" X%-2d 0x%016lx %ld => (function) %s\n", i, reg, reg, name.value().c_str());
                        } else {
                            REPORT_APPEND(" X%-2d 0x%016lx %ld => (function) %s+%ld\n", i, reg, reg, name.value().c_str(), offset);
                        }
                    } else if (isAscii((char*)reg)) {
                        REPORT_APPEND(" X%-2d 0x%016lx %ld => (string) %.32s\n", i, reg, reg, (char*)reg);
                    } else {
                        REPORT_APPEND(" X%-2d 0x%016lx %ld => (???) %s:0x%lx\n", i, reg, reg, mod->getModuleName(), reg - mod->range().start());
                    }

                    found = true;
                }
            }

            if (!found) {
                REPORT_APPEND(" X%-2d 0x%016lx %ld\n", i, reg, reg);
            }
        }
        REPORT_APPEND(" FP  0x%016lx %ld\n", info->FP.x, info->FP.x);
        REPORT_APPEND(" LR  0x%016lx %ld\n", info->LR.x, info->LR.x);
        REPORT_APPEND(" SP  0x%016lx %ld\n", info->SP.x, info->SP.x);

        auto found = false;
        for (int i = 0; i < hk::ro::getNumModules() && !found; ++i) {
            auto mod = hk::ro::getModuleByIndex(i);
            if (mod->range().start() <= info->PC.x && info->PC.x < mod->range().end()) {
                // Check ptr for:
                // - Functions
                // - Strings
                auto [name, offset] = functionAt(info->PC.x);
                if (name.has_value()) {
                    if (offset == 0) {
                        REPORT_APPEND(" PC  0x%016lx %ld => %s\n", info->PC.x, info->PC.x, name.value().c_str());
                    } else {
                        REPORT_APPEND(" PC  0x%016lx %ld => %s+%ld\n", info->PC.x, info->PC.x, name.value().c_str(), offset);
                    }
                    found = true;
                }
            }
        }

        if (!found) {
            REPORT_APPEND(" PC  0x%016lx %ld\n", info->PC.x, info->PC.x);
        }

        if (info->PC.x != 0) {
            REPORT_APPEND("  %s\n", disasm(reinterpret_cast<uint32_t*>(info->PC.x - 4)).c_str());
        }

        REPORT_APPEND(" FAR 0x%016lx %ld\n", info->FAR.x, info->FAR.x);
    } else {
        os << "Registers: [unknown]\n";
    }

    // FIXME: Crashes on render thread
    if (resCode == -1) {
        Logger::log("!!!Abort!!!\n%s",  exceptionBuffer.get().c_str());
        MessageUtil::popup(resCode, "!!!Abort!!!", exceptionBuffer.get());
    } else {
        auto mod = reinterpret_cast<nn::Result&>(resCode).GetModule() | 0x2000;
        auto desc = reinterpret_cast<nn::Result&>(resCode).GetDescription();

        FixedBuffer<32> title;
        std::ostream os(&title);

        REPORT_APPEND("Error: [%04x-%04x]", mod, desc);

        Logger::log("Error: [%04x-%04x]\n%s", mod, desc, exceptionBuffer.get().c_str());
        MessageUtil::popup(resCode, title.get(), exceptionBuffer.get());
    }
}

#ifdef __RTLD_PAST_13XX__
HkTrampoline<void, char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, char const*, std::va_list&> AbortHook = hk::hook::trampoline([](char const* cond, char const* func, char const* file, int code, nn::Result const* res, nn::os::UserExceptionInfo const* info, char const* fmt, std::va_list& args) {
#else
HkTrampoline<void, char const*, char const*, char const*, int, nn::Result const*, nn::os::UserExceptionInfo const*, nn::diag::MetaLogContext const&, char const*, std::va_list> AbortHook = hk::hook::trampoline([](char const* cond, char const* func, char const* file, int code, nn::Result const* res, nn::os::UserExceptionInfo const* info, nn::diag::MetaLogContext const& ctx, char const* fmt, std::va_list args) {
#endif
    const char* funcLog;
    const char* fileLog;
    const char* reasonLog;
    if (func == nullptr || strlen(func) == 0) {
        funcLog = "[unknown function]";  // TODO: Print offset from module?
    } else {
        funcLog = func;
    }
    if (file == nullptr || strlen(file) == 0) {
        fileLog = "[unknown file]";
    } else {
        fileLog = file;
    }
    if (cond == nullptr || strlen(cond) == 0) {
        reasonLog = "[unknown reason]";
    } else {
        reasonLog = cond;
    }

    auto errCode = res->GetInnerValueForDebug();
    if (errCode == -1) {
        errCode = -2;
    }

    reportException(info, funcLog, fileLog, code, reasonLog, errCode);

#ifdef __RTLD_PAST_13XX__
    AbortHook.orig(cond, func, file, code, res, info, fmt, args);
#else
    AbortHook.orig(cond, func, file, code, res, info, ctx, fmt, args);
#endif
});

alignas(0x1000) static char exceptionStack[0x8000];
static nn::os::UserExceptionInfo exceptionStorage;
void handleUserException(nn::os::UserExceptionInfo* info) {
    reportException(info, nullptr, nullptr, 0, nullptr, -1);
    nn::os::DefaultUserExceptionHandler(info);
}


void error_init() {
    AbortHook.installAtPtr(&nn::diag::detail::VAbortImpl);

    nn::os::SetUserExceptionHandler(&handleUserException, &exceptionStack, sizeof(exceptionStack), &exceptionStorage);
    nn::os::EnableUserExceptionHandlerOnDebugging(true);
}
