#include <hk/hook/Trampoline.h>
#include <logger/logger.h>
#include <nn/fs/fs_directories.h>
#include <nn/fs/fs_files.h>
#include <nn/fs/fs_types.h>
#include "game_constants.h"
#include "nn/ro.h"
#include "util/CodePatcher.h"

namespace yyc {
    extern void* YYConsoleOutput;
}

void YYConsoleOutputStub(const char* fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    auto size = vsnprintf(nullptr, 0, fmt, args_copy);
    char buffer[size + 2];
    vsnprintf(buffer, size + 1, fmt, args);
    if (buffer[size - 1] != '\n') {
        buffer[size] = '\n';
        buffer[size + 1] = 0;
    }
    Logger::log("YYConsoleOutput: %s", buffer);
}

HkTrampoline<void> fixYYConsoleOutput = hk::hook::trampoline([]() {
    yyc::YYConsoleOutput = (void*)&YYConsoleOutputStub;
    fixYYConsoleOutput.orig();
});

static void inlineLogHook(void*, char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    auto size = vsnprintf(nullptr, 0, fmt, args_copy);
    char buffer[size + 2];
    vsnprintf(buffer, size + 1, fmt, args);
    if (buffer[size - 1] != '\n') {
        buffer[size] = '\n';
        buffer[size + 1] = 0;
    }

    Logger::log("YYC Debug: %s", buffer);

    va_end(args);
}

HkReplaceVarArgs<void, void*, char*> logHook(inlineLogHook);

HkTrampoline<void> preventFSAllocator = hk::hook::replace([]() {

});

void logging_hooks() {
    preventFSAllocator.installAtSym<"_ZN2nn2fs12SetAllocatorEPFPvmEPFvS1_mE">();
    fixYYConsoleOutput.installAtMainOffset(0x00b0c090);  // Called after discovering YYConsoleOutput
    logHook.installAtMainOffset(0x008cea04);  // gml_stub_debugLog

    CodePatcher cp(0x009a2f34);
    cp.write<int>(0x52800000);
}
