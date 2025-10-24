#pragma once

namespace gfl {
    template <typename K, typename V, typename S = uint64_t>
    struct Map {
        struct Entry {
            Entry* next;
            S hash;
            K key;
            V value;
        };
        Entry* entries;
        // TODO: Other fields
    };

    template <typename T>
    struct Set {
        struct Entry {
            Entry* next;
            ulong hash;
            T value;
        };
        Entry* entries;
        // TODO: Other fields
    };

    template <typename T>
    struct Vector {
        T* m_begin;
        T* m_end;
        void** unk;
        gfl::SizedHeap* m_heap;
    };
}
