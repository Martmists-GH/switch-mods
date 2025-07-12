#pragma once
#include <codecvt>
#include <locale>

#include "externals/util.h"
#include "externals/gfl/object.h"

template<class Facet>
struct string16_deletable_facet : Facet {
    template<class... Args>
    string16_deletable_facet(Args &&... args) : Facet(std::forward<Args>(args)...) {}

    ~string16_deletable_facet() {}
};

// NS inferred from generated_gfl_text_StrBuf
namespace gfl::text {
    struct StrBuf : ExternalType<StrBuf> {
        struct vtable : gfl::ReferenceObject::vtable {
            // TODO
        };

        struct fields : gfl::ReferenceObject::fields {
            char16_t* m_buffer;
            uint16_t m_numChars;
            uint16_t m_numBytes;
            bool m_available;
            char _pad[0x3];
        };

        static_assert(offsetof(fields, m_buffer) == 0x40);
        static_assert(sizeof(fields) == 0x50);

        void $ctor(const char16_t* text);

        void $ctor(const char* text) {
            std::wstring_convert<string16_deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> conv16;
            std::u16string str16 = conv16.from_bytes(text);
            this->$ctor(str16.c_str());
        }

        std::string asString() {
            return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(impl()->fields.m_buffer);
        }
    };
}
