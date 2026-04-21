#include "MemoryBuffer.h"

#ifdef IMGUI_ENABLED
#include <nn/primitives.h>

#include "imgui_impl_nvn.hpp"
#include "logger/logger.h"

MemoryBuffer::MemoryBuffer(size_t size) {

    auto *bd = ImguiNvnBackend::getBackendData();

    size_t alignedSize = ALIGN_UP(size, 0x1000);

    memBuffer = aligned_alloc(0x1000, alignedSize);
    memset(memBuffer, 0, alignedSize);

    bd->memPoolBuilder.SetDefaults()
            .SetDevice(bd->device)
            .SetFlags(nvn::MemoryPoolFlags::CPU_UNCACHED | nvn::MemoryPoolFlags::GPU_CACHED)
            .SetStorage(memBuffer, alignedSize);

    if (!pool.Initialize(&bd->memPoolBuilder)) {
        Logger::log("Failed to Create Memory Pool! (1)\n");
        Logger::log("Buffer: %p", memBuffer);
        Logger::log("Aligned Buffer: %p", memBuffer, 0x1000);
        return;
    }

    bd->bufferBuilder.SetDevice(bd->device).SetDefaults().SetStorage(&pool, 0, alignedSize);

    if (!buffer.Initialize(&bd->bufferBuilder)) {
        Logger::log("Failed to Init Buffer!\n");
        return;
    }

    mIsReady = true;
}

MemoryBuffer::MemoryBuffer(size_t size, nvn::MemoryPoolFlags flags) {

    auto *bd = ImguiNvnBackend::getBackendData();

    size_t alignedSize = ALIGN_UP(size, 0x1000);
    memBuffer = aligned_alloc(0x1000, alignedSize);
    memset(memBuffer, 0, alignedSize);

    bd->memPoolBuilder.SetDefaults()
            .SetDevice(bd->device)
            .SetFlags(flags)
            .SetStorage(memBuffer, alignedSize);


    if (!pool.Initialize(&bd->memPoolBuilder)) {
        Logger::log("Failed to Create Memory Pool! (2)\n");
        return;
    }

    bd->bufferBuilder.SetDevice(bd->device).SetDefaults().SetStorage(&pool, 0, alignedSize);

    if (!buffer.Initialize(&bd->bufferBuilder)) {
        Logger::log("Failed to Init Buffer!\n");
        return;
    }

    mIsReady = true;
}

MemoryBuffer::MemoryBuffer(size_t size, void *bufferPtr, nvn::MemoryPoolFlags flags) {

    auto *bd = ImguiNvnBackend::getBackendData();

    memBuffer = bufferPtr;

    bd->memPoolBuilder.SetDefaults()
            .SetDevice(bd->device)
            .SetFlags(flags)
            .SetStorage(memBuffer, size);

    if (!pool.Initialize(&bd->memPoolBuilder)) {
        Logger::log("Failed to Create Memory Pool! (3)\n");
        return;
    }

    bd->bufferBuilder.SetDevice(bd->device).SetDefaults().SetStorage(&pool, 0, size);

    if (!buffer.Initialize(&bd->bufferBuilder)) {
        Logger::log("Failed to Init Buffer!\n");
        return;
    }

    mIsReady = true;
}

void MemoryBuffer::Finalize() {
    free(memBuffer);
    pool.Finalize();
    buffer.Finalize();
}

void MemoryBuffer::ClearBuffer() {
    memset(memBuffer, 0, pool.GetSize());
}
#endif
