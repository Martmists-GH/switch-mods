#pragma once

#include <codecvt>
#include <locale>

#include "externals/gfl/object.h"

template<class Facet>
struct string16_deletable_facet : Facet {
    template<class... Args>
    string16_deletable_facet(Args &&... args) : Facet(std::forward<Args>(args)...) {}

    ~string16_deletable_facet() {}
};

namespace gfl::text {
    struct StrBuf : ExternalType<StrBuf> {
        struct vtable : gfl::ReferenceObject::vtable { };
        struct fields : gfl::ReferenceObject::fields {
            char16_t* m_buffer;
        };

        std::string asString() {
            return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(impl()->fields.m_buffer);
        }
    };
    static_assert(offsetof(StrBuf::instance, fields.m_buffer) == 0x30);
}
