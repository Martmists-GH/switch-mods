#pragma once
#include <externals/util.h>
#include "object.h"
#include "externals/gfl/hash.h"

namespace gfl {
    struct String : ExternalType<String> {
        struct vtable : gfl::ReferenceObject::vtable {};
        struct fields : gfl::ReferenceObject::fields {
            long m_hash;
            const char* m_string;
            size_t m_length;
        };
    };

    struct StringHolder {
        long m_hash;
        const char* m_string;
        size_t m_length;
        gfl::String* m_object;

        static StringHolder Create(const char* content) {
            auto holder = StringHolder { };
            holder.m_hash = gfl::FnvHash64::hash(content);
            holder.m_string = content;
            holder.m_length = strlen(content);
            holder.m_object = nullptr;
            return holder;
        }
    };
}
