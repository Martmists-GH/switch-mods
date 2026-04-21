#include "MemoryPoolMaker.h"

#ifdef IMGUI_ENABLED
#include "imgui_impl_nvn.hpp"
#include "logger/logger.h"

bool MemoryPoolMaker::createPool(nvn::MemoryPool *result, size_t size,
                                 const nvn::MemoryPoolFlags &flags) {

    auto bd = ImguiNvnBackend::getBackendData();

    void *poolPtr = aligned_alloc(0x1000, size);

    nvn::MemoryPoolBuilder memPoolBuilder{};
    memPoolBuilder.SetDefaults()
        .SetDevice(bd->device)
        .SetFlags(flags)
        .SetStorage(poolPtr, size);

    if (!result->Initialize(&memPoolBuilder)) {
        return false;
    }

    return true;
}
#endif
