#pragma once
#include <externals/stl.h>

namespace gfl {
    struct _DummyAllocator {
        EXTERNAL_PAD(8);
    };

    template <typename K, typename V, typename S = gfl::FnvHash64>
    using Map = stl::unordered_map<K, V, _DummyAllocator, S>;

    static_assert(sizeof(gfl::Map<int, int, int>) == 0x38);

    template <typename T, typename S = gfl::FnvHash64>
    using Set = stl::unordered_set<T, _DummyAllocator, S>;

    struct _VectorAllocator {
        EXTERNAL_PAD(8);
        gfl::SizedHeap* m_heap;
    };

    template <typename T>
    using Vector = stl::vector<T, _VectorAllocator>;
}
