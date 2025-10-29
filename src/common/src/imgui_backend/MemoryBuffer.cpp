#include "MemoryBuffer.h"

#ifdef IMGUI_ENABLED
#include <nn/primitives.h>

#include "imgui_impl_nvn.hpp"
#include "logger/logger.h"

MemoryBuffer::MemoryBuffer(size_t size) {

    auto *bd = ImguiNvnBackend::getBackendData();

    size_t alignedSize = ALIGN_UP(size, 0x1000);

    memBuffer = IM_ALLOC(alignedSize + 0x1000);
    memset(memBuffer, 0, alignedSize + 0x1000);

    bd->memPoolBuilder.SetDefaults()
            .SetDevice(bd->device)
            .SetFlags(nvn::MemoryPoolFlags::CPU_UNCACHED | nvn::MemoryPoolFlags::GPU_CACHED)
            .SetStorage((void*)ALIGN_UP(memBuffer, 0x1000), alignedSize);

    if (!pool.Initialize(&bd->memPoolBuilder)) {
        Logger::log("Failed to Create Memory Pool! (1)\n");
        Logger::log("Buffer: %p", memBuffer);
        Logger::log("Aligned Buffer: %p", ALIGN_UP(memBuffer, 0x1000));
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
    memBuffer = IM_ALLOC(alignedSize + 0x1000);
    memset(memBuffer, 0, alignedSize + 0x1000);

    bd->memPoolBuilder.SetDefaults()
            .SetDevice(bd->device)
            .SetFlags(flags)
            .SetStorage((void*)ALIGN_UP(memBuffer, 0x1000), alignedSize);


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
    IM_FREE(memBuffer);
    pool.Finalize();
    buffer.Finalize();
}

void MemoryBuffer::ClearBuffer() {
    memset(memBuffer, 0, pool.GetSize());
}
#endif
