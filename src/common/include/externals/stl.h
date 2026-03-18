#pragma once

namespace stl {
    template<typename K, typename V, typename Allocator, typename hasher = std::hash<K>>
    struct unordered_map {
        struct Entry {
            Entry* next;
            long hash;
            K key;
            V value;
        };

        Entry** bucketList;
        long numBuckets;
        Allocator listAllocator;
        Entry* first;
        Allocator firstAllocator;
        long size;
        float max_load_factor;

        V* Get(const K& key) {
            hasher fn;
            return GetByHash(fn(key));
        }

        V* GetByHash(long hash) {
            auto bucket = bucketList[hash % numBuckets];
            while (bucket) {
                if (bucket->hash == hash) {
                    return &bucket->value;
                }
                bucket = bucket->next;
            }
            return nullptr;
        }
    };

    template<typename T, typename Allocator, typename hasher = std::hash<T>>
    struct unordered_set {
        struct Entry {
            Entry* next;
            long hash;
            T value;
        };

        Entry** bucketList;
        long numBuckets;
        Allocator listAllocator;
        Entry* first;
        Allocator firstAllocator;
        long size;
        float max_load_factor;

        bool Contains(const T& value) {
            hasher fn;
            return GetByHash(fn(value)) != nullptr;
        }

        T* GetByHash(long hash) {
            auto bucket = bucketList[hash % numBuckets];
            while (bucket) {
                if (bucket->hash == hash) {
                    return &bucket->value;
                }
                bucket = bucket->next;
            }
            return nullptr;
        }
    };

    template <typename T, typename Allocator>
    struct vector {
        T* m_begin;
        T* m_end;
        Allocator m_allocator;

        size_t size() {
            auto diff_bytes = ((uintptr_t)m_end) - ((uintptr_t)m_begin);
            return diff_bytes / sizeof(T);
        }

        T* Get(int index) {
            if (index < 0 || index >= size()) {
                return nullptr;
            }
            return &m_begin[index];
        }
    };
}
