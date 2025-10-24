#pragma once
#include <externals/util.h>
#include "object.h"
#include "externals/gfl/hash.h"

namespace gfl {
    struct StringContent {
        long m_hash;
        const char* m_string;
        size_t m_length;
    };

    struct String : ExternalType<String> {
        struct vtable : gfl::ReferenceObject::vtable {};
        struct fields : gfl::ReferenceObject::fields {
            gfl::StringContent m_content;
        };
    };
    static_assert(offsetof(String::instance, fields.m_content) == 0x30);

    // static_assert(offsetof(String::instance, fields.m_content) == 0x30);
    // static_assert(offsetof(String::instance, fields.m_content.m_string) == 0x38);

    struct StringHolder {
        gfl::StringContent m_content;
        gfl::String* m_object;

        static StringHolder Create(const char* content) {
            auto holder = StringHolder { };
            holder.m_content.m_hash = gfl::FnvHash64::hash(content);
            holder.m_content.m_string = content;
            holder.m_content.m_length = strlen(content);
            holder.m_object = nullptr;
            return holder;
        }
    };
}
