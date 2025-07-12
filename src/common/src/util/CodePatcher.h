#pragma once
#include <hk/hook/a64/Assembler.h>
#include <hk/hook/a64/Assembler.h>


class CodePatcher {
    uintptr_t offset;
    const hk::ro::RoModule* module;

public:
    CodePatcher(void* ptr) : offset(reinterpret_cast<uintptr_t>(ptr) - hk::ro::getMainModule()->range().start()), module(hk::ro::getMainModule()) {}
    CodePatcher(uintptr_t offset) : offset(offset), module(hk::ro::getMainModule()) {}
    CodePatcher(uintptr_t offset, const hk::ro::RoModule* module) : offset(offset), module(module) {}

    template <typename T>
    void write(const T& value) {
        module->writeRo(offset, value);
        offset += sizeof(T);
    }

    template <bool Uninstallable, size NumInstrs, size MaxArgs>
    void write(hk::hook::a64::AsmBlock<Uninstallable, NumInstrs, MaxArgs> expr) {
        expr.installAtOffset(module, offset);
        offset += sizeof(hk::hook::Instr) * NumInstrs;
    }

    template <hk::hook::a64::Expr E, bool Uninstallable = false, int N = E.calcNumInstrs()>
    void write() {
        write(hk::hook::a64::assemble<E, Uninstallable, N>());
    }
};
