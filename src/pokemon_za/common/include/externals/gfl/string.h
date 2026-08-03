#pragma once
#include <externals/util.h>
#include "object.h"
#include "externals/gfl/hash.h"

namespace gfl {
    struct StaticString {
        uint64_t m_hash;
        const char* m_string;
        size_t m_length;
    };

    namespace impl {
        struct String : ExternalType<String> {
            struct vtable : gfl::ReferenceObject<>::vtable {};
            struct fields : gfl::ReferenceObject<>::fields {
                gfl::StaticString m_content;
            };
        };

        struct PooledString {

        };
    }

    struct String {
        gfl::impl::PooledString* m_string;
    };

    // static_assert(offsetof(String::instance, fields.m_content) == 0x30);
    // static_assert(offsetof(String::instance, fields.m_content.m_string) == 0x38);

    struct StringView {
        gfl::StaticString m_content;
        gfl::String m_object;

        static StringView Create(const char* content) {
            auto holder = StringView { };
            holder.m_content.m_hash = gfl::FnvHash64::hash(content);
            holder.m_content.m_string = content;
            holder.m_content.m_length = strlen(content);
            holder.m_object.m_string = nullptr;
            return holder;
        }
    };
}
