#pragma once

namespace pml::pokepara {
    struct CoreParam : ExternalType<CoreParam> {
        struct vtable : gfl::ReferenceObject::vtable {};
        struct fields : gfl::ReferenceObject::fields {};

        bool IsRare();
        bool IsOybn();
        int GetMonsNo();
        int GetFormNo();

        void SetIsOybn(bool value);
        void SetMonsNo(int value);
        void SetFormNo(int value);
    };
};
