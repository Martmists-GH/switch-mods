#include "imgui_backend_v2/ImGuiBackend.h"

#include <nn/primitives.h>

#define UBOSIZE 0x1000
#define MEGABYTE (1024 * 1024)
#define POOL_SIZE ALIGN_UP(UBOSIZE, MEGABYTE)

// ImGuiBackend::ImGuiBackend(nvn::Device* device) : m_device(device) {
//     {
//         nvn::QueueBuilder builder{};
//         builder
//             .SetDevice(device)
//             .SetDefaults()
//             .SetFlags(nvn::QueueFlags::NO_ZCULL);
//
//         m_queue.Initialize(&builder);
//     }
//
//     m_commandBufferData.m_cmdPool = aligned_alloc(MEGABYTE, POOL_SIZE);
//
//     {
//         nvn::MemoryPoolBuilder builder{};
//         builder
//             .SetDevice(device)
//             .SetFlags(nvn::MemoryPoolFlags::CPU_UNCACHED | nvn::MemoryPoolFlags::GPU_CACHED | nvn::MemoryPoolFlags::SHADER_CODE)
//             .SetStorage(m_commandBufferData.m_cmdPool, UBOSIZE);
//         m_commandBufferData.m_pool.Initialize(&builder);
//     }
//
//     initializeImGui();
//     initializeShaders();
//     initializeFonts();
// }
//
// ImGuiBackend::~ImGuiBackend() {
//     m_queue.Finalize();
//     free(m_commandBufferData.m_cmdPool);
//     m_commandBufferData.m_pool.Finalize();
// }
//
// void ImGuiBackend::initializeImGui() {
//     // TODO
// }
//
// void ImGuiBackend::initializeShaders() {
//     // TODO
// }
//
//
// void ImGuiBackend::initializeFonts() {
//     // TODO
// }
//
// void ImGuiBackend::collectInputs() {
//     // TODO
// }
//
//
// void ImGuiBackend::render() {
//     // TODO
// }
