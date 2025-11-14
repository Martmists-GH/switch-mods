#pragma once
#include <functional>
#include <imgui.h>
#include <vector>
#include <nvn/nvn_Cpp.h>

class ImGuiBackend {
    nvn::Device* m_device;
    nvn::Queue m_queue;
    nvn::CommandBuffer m_commandBuffer;
    struct {
        nvn::MemoryPool m_pool;
        void* m_cmdPool;
    } m_commandBufferData;
    std::vector<std::function<void(const nvn::CommandBuffer&, ImVec2)>> m_renderCallbacks;

    void initializeImGui();
    void initializeShaders();
    void initializeFonts();

    void collectInputs();

public:
    explicit ImGuiBackend(nvn::Device* device);
    ~ImGuiBackend();

    void render();

    void addRenderCallback(std::function<void(const nvn::CommandBuffer&, ImVec2)>&& callback) {
        m_renderCallbacks.push_back(callback);
    }
};
