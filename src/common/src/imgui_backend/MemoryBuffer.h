#pragma once

#include "nn/types.h"
#include "nvn/nvn_Cpp.h"

class MemoryBuffer {
public:
    explicit MemoryBuffer(size_t size);

    explicit MemoryBuffer(size_t size, nvn::MemoryPoolFlags flags);

    explicit MemoryBuffer(size_t size, void *buffer, nvn::MemoryPoolFlags flags);

    void Finalize();

    size_t GetPoolSize() const { return pool.GetSize(); }

    nvn::BufferAddress GetBufferAddress() const { return buffer.GetAddress(); };

    u8 *GetMemPtr() {
        if (mMemPtr == nullptr) {
            mMemPtr = pool.Map();
        }
        return (u8*) mMemPtr;
    };

    bool IsBufferReady() { return mIsReady; }

    void ClearBuffer();

    operator nvn::BufferAddress() const { return buffer.GetAddress(); }

private:
    nvn::MemoryPool pool;
    nvn::Buffer buffer;

    void *memBuffer;
    void * mMemPtr = nullptr;
    bool mIsReady = false;
};
