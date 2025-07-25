#include <cmath>
#include <cstring>
#include "imgui_impl_nvn.hpp"

#include <util/FileUtil.h>

#include "imgui_hid_mappings.h"

#include "nn/os.h"
#include "nn/hid.h"

#include "util/InputUtil.h"
#include "MemoryPoolMaker.h"
#include "nn/oe.h"
#include "logger/logger.h"
#include "checks.hpp"
#include <nn/primitives.h>
#include "util/common_utils.h"

#include "game_constants.h"

#define UBOSIZE 0x1000

typedef float Matrix44f[4][4];

// orthographic matrix used for shader
static const Matrix44f projMatrix = {
        {0.001563f, 0.0f,       0.0f,  0.0f},
        {0.0f,      -0.002778f, 0.0f,  0.0f},
        {0.0f,      0.0f,       -0.5f, 0.0f},
        {-1.0f,     1.0f,       0.5f,  1.0f}
};

namespace ImguiNvnBackend {
    // neat tool to cycle through all loaded textures in a texture pool
    int texIDSelector() {
        {

            static int curId = 256;
            static int downCounter = 0;
            static int upCounter = 0;

            if (InputUtil::isButtonPress(nn::hid::NpadButton::Left)) {
                curId--;
                Logger::log("ID: %d\n", curId);
            } else if (InputUtil::isButtonHold(nn::hid::NpadButton::Left)) {

                downCounter++;
                if (downCounter > 30) {
                    curId--;
                    Logger::log("ID: %d\n", curId);
                }
            } else {
                downCounter = 0;
            }

            if (InputUtil::isButtonPress(nn::hid::NpadButton::Right)) {
                curId++;
                Logger::log("ID: %d\n", curId);
            } else if (InputUtil::isButtonHold(nn::hid::NpadButton::Right)) {

                upCounter++;
                if (upCounter > 30) {
                    curId++;
                    Logger::log("ID: %d\n", curId);
                }
            } else {
                upCounter = 0;
            }

            /* fun values with bd->device->GetTextureHandle(curId, 256):
             * 282 = Window Texture
             * 393 = Some sort of render pass (shadow?) nvm it just seems to be the first occurrence of many more textures like it
             * 257 = debug font texture
             */

            return curId;
        }
    }

    // places ImDrawVerts, starting at startIndex, that use the x,y,width, and height values to define vertex coords
    void createQuad(ImDrawVert *verts, int startIndex, int x, int y, int width, int height, ImU32 col) {

        float minXVal = x;
        float maxXVal = x + width;
        float minYVal = y; // 400
        float maxYVal = y + height; // 400

        // top left
        ImDrawVert p1 = {
                .pos = ImVec2(minXVal, minYVal),
                .uv = ImVec2(0.0f, 0.0f),
                .col = col
        };
        // top right
        ImDrawVert p2 = {
                .pos = ImVec2(minXVal, maxYVal),
                .uv = ImVec2(0.0f, 1.0f),
                .col = col
        };
        // bottom left
        ImDrawVert p3 = {
                .pos = ImVec2(maxXVal, minYVal),
                .uv = ImVec2(1.0f, 0.0f),
                .col = col
        };
        // bottom right
        ImDrawVert p4 = {
                .pos = ImVec2(maxXVal, maxYVal),
                .uv = ImVec2(1.0f, 1.0f),
                .col = col
        };

        verts[startIndex] = p4;
        verts[startIndex + 1] = p2;
        verts[startIndex + 2] = p1;

        verts[startIndex + 3] = p1;
        verts[startIndex + 4] = p3;
        verts[startIndex + 5] = p4;
    }

    // this function is mainly what I used to debug the rendering of ImGui, so code is a bit messier
    void renderTestShader(ImDrawData *drawData) {

        auto bd = getBackendData();
        auto io = ImGui::GetIO();

        constexpr int triVertCount = 3;
        constexpr int quadVertCount = triVertCount * 2;

        int quadCount = 1; // modify to reflect how many quads need to be drawn per frame

        int pointCount = quadVertCount * quadCount;

        size_t totalVtxSize = pointCount * sizeof(ImDrawVert);
        if (!bd->vtxBuffer || bd->vtxBuffer->GetPoolSize() < totalVtxSize) {
            if (bd->vtxBuffer) {
                bd->vtxBuffer->Finalize();
                IM_FREE(bd->vtxBuffer);
            }
            bd->vtxBuffer = IM_NEW(MemoryBuffer)(totalVtxSize);
            Logger::log("(Re)sized Vertex Buffer to Size: %d\n", totalVtxSize);
        }

        if (!bd->vtxBuffer->IsBufferReady()) {
            Logger::log("Cannot Draw Data! Buffers are not Ready.\n");
            return;
        }

        ImDrawVert *verts = (ImDrawVert *) bd->vtxBuffer->GetMemPtr();

        float scale = 3.0f;

        float imageX = 1 * scale; // bd->fontTexture.GetWidth();
        float imageY = 1 * scale; // bd->fontTexture.GetHeight();

        createQuad(verts, 0, (io.DisplaySize.x / 2) - (imageX), (io.DisplaySize.y / 2) - (imageY), imageX, imageY,
                   IM_COL32_WHITE);

        bd->cmdBuf->BeginRecording();
        bd->cmdBuf->BindProgram(&bd->shaderProgram, nvn::ShaderStageBits::VERTEX | nvn::ShaderStageBits::FRAGMENT);

        bd->cmdBuf->BindUniformBuffer(nvn::ShaderStage::VERTEX, 0, *bd->uniformMemory, UBOSIZE);
        bd->cmdBuf->UpdateUniformBuffer(*bd->uniformMemory, UBOSIZE, 0, sizeof(projMatrix), &projMatrix);

        bd->cmdBuf->BindVertexBuffer(0, (*bd->vtxBuffer), bd->vtxBuffer->GetPoolSize());

        setRenderStates();

//        bd->cmdBuf->BindTexture(nvn::ShaderStage::FRAGMENT, 0, bd->fontTexHandle);

        bd->cmdBuf->DrawArrays(nvn::DrawPrimitive::TRIANGLES, 0, pointCount);

        auto handle = bd->cmdBuf->EndRecording();
        bd->queue->SubmitCommands(1, &handle);
    }

// backend impl

    NvnBackendData *getBackendData() {
        NvnBackendData *result = ImGui::GetCurrentContext() ? (NvnBackendData *) ImGui::GetIO().BackendRendererUserData
                                                            : nullptr;
        HK_ASSERT_MSG(result, "Backend has not been initialized!");
        return result;
    }

    bool createShaders() {

        Logger::log("Creating/Loading Shaders.\n");

        auto bd = getBackendData();

        Logger::log("Unable to compile shaders at runtime. falling back to pre-compiled shaders.\n");

        auto handle = FileUtil::readFile(ROM_MOUNT ":/shaders/imgui.bin", 0x1000);
        if (handle.has_value()) {
            auto h = std::move(handle.value());
            bd->imguiShaderBinary.size = h.size;
            bd->imguiShaderBinary.ptr = std::static_pointer_cast<u8>(h.buffer);
        }

        if (bd->imguiShaderBinary.size > 0) {
            return true;
        }

        return false;
    }

    bool setupFont() {

        Logger::log("Setting up ImGui Font.\n");

        auto bd = getBackendData();

        ImGuiIO &io = ImGui::GetIO();

        // init sampler and texture pools

        int sampDescSize = 0;
        bd->device->GetInteger(nvn::DeviceInfo::SAMPLER_DESCRIPTOR_SIZE, &sampDescSize);
        int texDescSize = 0;
        bd->device->GetInteger(nvn::DeviceInfo::TEXTURE_DESCRIPTOR_SIZE, &texDescSize);

        int sampMemPoolSize = sampDescSize * MaxSampDescriptors;
        int texMemPoolSize = texDescSize * MaxTexDescriptors;
        int totalPoolSize = ALIGN_UP(sampMemPoolSize + texMemPoolSize, 0x1000);
        if (!MemoryPoolMaker::createPool(&bd->sampTexMemPool, totalPoolSize)) {
            Logger::log("Failed to Create Texture/Sampler Memory Pool!\n");
            return false;
        }

        if (!bd->samplerPool.Initialize(&bd->sampTexMemPool, 0, MaxSampDescriptors)) {
            Logger::log("Failed to Create Sampler Pool!\n");
            return false;
        }

        if (!bd->texPool.Initialize(&bd->sampTexMemPool, sampMemPoolSize, MaxTexDescriptors)) {
            Logger::log("Failed to Create Texture Pool!\n");
            return false;
        }

        // convert imgui font texels

        unsigned char *pixels;
        int width, height, pixelByteSize;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &pixelByteSize);
        int texPoolSize = pixelByteSize * width * height;

        if (!MemoryPoolMaker::createPool(&bd->fontMemPool, ALIGN_UP(texPoolSize, 0x1000),
                                         nvn::MemoryPoolFlags::CPU_UNCACHED | nvn::MemoryPoolFlags::GPU_CACHED)) {
            Logger::log("Failed to Create Font Memory Pool!\n");
            return false;
        }

        bd->texBuilder.SetDefaults()
                .SetDevice(bd->device)
                .SetTarget(nvn::TextureTarget::TARGET_2D)
                .SetFormat(nvn::Format::RGBA8)
                .SetSize2D(width, height)
                .SetStorage(&bd->fontMemPool, 0);

        if (!bd->fontTexture.Initialize(&bd->texBuilder)) {
            Logger::log("Failed to Create Font Texture!\n");
            return false;
        }

        // setup font texture

        nvn::CopyRegion region = {
                .xoffset = 0,
                .yoffset = 0,
                .zoffset = 0,
                .width = bd->fontTexture.GetWidth(),
                .height = bd->fontTexture.GetHeight(),
                .depth = 1
        };

        bd->fontTexture.WriteTexels(nullptr, &region, pixels);
        bd->fontTexture.FlushTexels(nullptr, &region);

        bd->samplerBuilder.SetDefaults()
                .SetDevice(bd->device)
                .SetMinMagFilter(nvn::MinFilter::LINEAR, nvn::MagFilter::LINEAR)
                .SetWrapMode(nvn::WrapMode::CLAMP, nvn::WrapMode::CLAMP, nvn::WrapMode::CLAMP);

        if (!bd->fontSampler.Initialize(&bd->samplerBuilder)) {
            Logger::log("Failed to Init Font Sampler!\n");
            return false;
        }

        bd->textureId = 257;
        bd->samplerId = 257;

        bd->texPool.RegisterTexture(bd->textureId, &bd->fontTexture, nullptr);
        bd->samplerPool.RegisterSampler(bd->samplerId, &bd->fontSampler);

        bd->fontTexHandle = bd->device->GetTextureHandle(bd->textureId, bd->samplerId);
        io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(&bd->fontTexHandle));

        Logger::log("Finished.\n");

        return true;
    }

    bool setupShaders(u8 *shaderBinary, ulong binarySize) {

        Logger::log("Setting up ImGui Shaders.\n");

        auto bd = getBackendData();

        if (!bd->shaderProgram.Initialize(bd->device)) {
            Logger::log("Failed to Initialize Shader Program!");
            return false;
        }

        bd->shaderMemory = IM_NEW(MemoryBuffer)(binarySize, nvn::MemoryPoolFlags::CPU_UNCACHED |
                                                                          nvn::MemoryPoolFlags::GPU_CACHED |
                                                                          nvn::MemoryPoolFlags::SHADER_CODE);

        if (!bd->shaderMemory->IsBufferReady()) {
            Logger::log("Shader Memory Pool not Ready! Unable to continue.\n");
            return false;
        }

        memcpy(bd->shaderMemory->GetMemPtr(), shaderBinary, binarySize);
        BinaryHeader offsetData = BinaryHeader((u32 *) shaderBinary);

        nvn::BufferAddress addr = bd->shaderMemory->GetBufferAddress();

        nvn::ShaderData &vertShaderData = bd->shaderDatas[0];
        vertShaderData.data = addr + offsetData.mVertexDataOffset;
        vertShaderData.control = shaderBinary + offsetData.mVertexControlOffset;

        nvn::ShaderData &fragShaderData = bd->shaderDatas[1];
        fragShaderData.data = addr + offsetData.mFragmentDataOffset;
        fragShaderData.control = shaderBinary + offsetData.mFragmentControlOffset;

        if (!bd->shaderProgram.SetShaders(2, bd->shaderDatas)) {
            Logger::log("Failed to Set shader data for program.\n");
            return false;
        }

        bd->shaderProgram.SetDebugLabel("ImGuiShader");

        // Uniform Block Object Memory Setup

        bd->uniformMemory = IM_NEW(MemoryBuffer)(UBOSIZE);

        if (!bd->uniformMemory->IsBufferReady()) {
            Logger::log("Uniform Memory Pool not Ready! Unable to continue.\n");
            return false;
        }

        // setup vertex attrib & stream

        bd->attribStates[0].SetDefaults().SetFormat(nvn::Format::RG32F, offsetof(ImDrawVert, pos)); // pos
        bd->attribStates[1].SetDefaults().SetFormat(nvn::Format::RG32F, offsetof(ImDrawVert, uv)); // uv
        bd->attribStates[2].SetDefaults().SetFormat(nvn::Format::RGBA8, offsetof(ImDrawVert, col)); // color

        bd->streamState.SetDefaults().SetStride(sizeof(ImDrawVert));

        Logger::log("Finished.\n");

        return true;
    }

    void InitBackend(const NvnBackendInitInfo &initInfo) {
        ImGuiIO &io = ImGui::GetIO();
        HK_ASSERT_MSG(!io.BackendRendererUserData, "Already Initialized Imgui Backend!");

        Logger::log("Setting Default Data/Config Data.\n");

        io.BackendPlatformName = "Switch";
        io.BackendRendererName = "imgui_impl_nvn";
        io.IniFilename = nullptr;
        io.MouseDrawCursor = !is_emulator();
        io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        io.DisplaySize = ImVec2(1280, 720); // default size

        Logger::log("Creating Backend Data.\n");

        auto *bd = IM_NEW(NvnBackendData)();
        HK_ASSERT_MSG(bd, "Backend was not Created!");

        io.BackendRendererUserData = (void *) bd;

        Logger::log("Setting Init Data into Backend Data.\n");

        bd->device = initInfo.device;
        bd->queue = initInfo.queue;
        bd->cmdBuf = initInfo.cmdBuf;
        bd->viewportSize = ImVec2(1280, 720);
        bd->isInitialized = false;

        Logger::log("Adding Default Font.\n");

        io.Fonts->AddFontDefault();

        Logger::log("Creating Shaders.\n");

        if (createShaders()) {
            Logger::log("Shader Binaries Loaded! Setting up Render Data.\n");

            if (setupShaders(bd->imguiShaderBinary.ptr.get(), bd->imguiShaderBinary.size) && setupFont()) {
                Logger::log("Rendering Setup!\n");

                bd->isInitialized = true;

            } else {
                Logger::log("Failed to Setup Render Data!\n");
            }
        }
    }

    void ShutdownBackend() {

    }

    void updateInput() {
        if(!InputUtil::isInputToggled()) {
            return;
        }

        ImGuiIO &io = ImGui::GetIO();

        for (auto [im_k, nx_k]: npad_mapping) {
            if (InputUtil::isButtonPress(static_cast<nn::hid::NpadButton>(nx_k)))
                io.AddKeyEvent(static_cast<ImGuiKey>(im_k), true);
            else if (InputUtil::isButtonRelease(static_cast<nn::hid::NpadButton>(nx_k)))
                io.AddKeyEvent(static_cast<ImGuiKey>(im_k), false);
        }

        InputUtil::getMouseCoords(&io.MousePos.x, &io.MousePos.y);

        ImVec2 scrollDelta(0, 0);

        InputUtil::getScrollDelta(&scrollDelta.x, &scrollDelta.y);

        if (scrollDelta.x > 0.0f || scrollDelta.y > 0.0f) {
            io.AddMouseWheelEvent(scrollDelta.x, scrollDelta.y);
        }

        for (auto [im_k, nx_k]: mouse_mapping) {
            if (InputUtil::isMousePress(static_cast<nn::hid::MouseButton>(nx_k)))
                io.AddMouseButtonEvent(im_k, true);
            else if (InputUtil::isMouseRelease(static_cast<nn::hid::MouseButton>(nx_k)))
                io.AddMouseButtonEvent(im_k, false);
        }

        for (auto [im_k, nx_k]: key_mapping) {
            if (InputUtil::isKeyPress(static_cast<nn::hid::KeyboardKey>(nx_k))) {
                io.AddKeyEvent(static_cast<ImGuiKey>(im_k), true);
            } else if (InputUtil::isKeyRelease(static_cast<nn::hid::KeyboardKey>(nx_k))) {
                io.AddKeyEvent(static_cast<ImGuiKey>(im_k), false);
            }
        }

    }

    void newFrame() {
        ImGuiIO &io = ImGui::GetIO();
        auto *bd = getBackendData();

        nn::TimeSpan curTick = nn::os::ConvertToTimeSpan(nn::os::GetSystemTick());
        nn::TimeSpan prevTick(bd->lastTick);
        io.DeltaTime = fabsf((curTick.nanoseconds - prevTick.nanoseconds) / 1e9f);

        bd->lastTick = curTick;

        updateInput(); // update backend inputs
    }

    void setRenderStates() {

        auto bd = getBackendData();

        nvn::PolygonState polyState;
        polyState.SetDefaults();
        polyState.SetPolygonMode(nvn::PolygonMode::FILL);
        polyState.SetCullFace(nvn::Face::NONE);
        polyState.SetFrontFace(nvn::FrontFace::CCW);
        bd->cmdBuf->BindPolygonState(&polyState);

        nvn::ColorState colorState;
        colorState.SetDefaults();
        colorState.SetLogicOp(nvn::LogicOp::COPY);
        colorState.SetAlphaTest(nvn::AlphaFunc::ALWAYS);
        for (int i = 0; i < 8; ++i) {
            colorState.SetBlendEnable(i, true);
        }
        bd->cmdBuf->BindColorState(&colorState);

        nvn::BlendState blendState;
        blendState.SetDefaults();
        blendState.SetBlendFunc(nvn::BlendFunc::SRC_ALPHA, nvn::BlendFunc::ONE_MINUS_SRC_ALPHA, nvn::BlendFunc::ONE,
                                nvn::BlendFunc::ZERO);
        blendState.SetBlendEquation(nvn::BlendEquation::ADD, nvn::BlendEquation::ADD);
        bd->cmdBuf->BindBlendState(&blendState);

        bd->cmdBuf->BindVertexAttribState(3, bd->attribStates);
        bd->cmdBuf->BindVertexStreamState(1, &bd->streamState);

        bd->cmdBuf->SetTexturePool(&bd->texPool);
        bd->cmdBuf->SetSamplerPool(&bd->samplerPool);
    }

    void renderDrawData(ImDrawData *drawData) {
        // defaults
        int width, height;
#ifdef IMGUI_720
        width = 1280;
        height = 720;
#else
        width = 1920;
        height = 1080;
#endif
//        if (displayPtr != nullptr) {  // I can guarantee this isn't null
//            nn::vi::GetDisplayResolution(&width, &height, displayPtr);
//            Logger::log("Display Resolution: %d x %d\n", width, height);
//        }

        // we dont need to process any data if it isnt valid
        if (!drawData->Valid) {
//            Logger::log("Draw Data was Invalid! Skipping Render.");
            return;
        }
        // if we dont have any command lists to draw, we can stop here
        if (drawData->CmdListsCount == 0) {
//            Logger::log("Command List was Empty! Skipping Render.\n");
            return;
        }

        // get both the main backend data and IO from ImGui
        auto bd = getBackendData();
        ImGuiIO &io = ImGui::GetIO();

        // if something went wrong during backend setup, don't try to render anything
        if (!bd->isInitialized) {
            Logger::log("Backend Data was not fully initialized!\n");
            return;
        }

        // initializes/resizes buffer used for all vertex data created by ImGui
        size_t totalVtxSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
        if (!bd->vtxBuffer || bd->vtxBuffer->GetPoolSize() < totalVtxSize) {
            if (bd->vtxBuffer) {
                bd->vtxBuffer->Finalize();
                IM_FREE(bd->vtxBuffer);
                Logger::log("Resizing Vertex Buffer to Size: %d\n", totalVtxSize);
            } else {
                Logger::log("Initializing Vertex Buffer to Size: %d\n", totalVtxSize);
            }

            bd->vtxBuffer = IM_NEW(MemoryBuffer)(totalVtxSize);
        }

        // initializes/resizes buffer used for all index data created by ImGui
        size_t totalIdxSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);
        if (!bd->idxBuffer || bd->idxBuffer->GetPoolSize() < totalIdxSize) {
            if (bd->idxBuffer) {

                bd->idxBuffer->Finalize();
                IM_FREE(bd->idxBuffer);

                Logger::log("Resizing Index Buffer to Size: %d\n", totalIdxSize);
            } else {
                Logger::log("Initializing Index Buffer to Size: %d\n", totalIdxSize);
            }

            bd->idxBuffer = IM_NEW(MemoryBuffer)(totalIdxSize);

        }

        // if we fail to resize/init either buffers, end execution before we try to use said invalid buffer(s)
        if (!(bd->vtxBuffer->IsBufferReady() && bd->idxBuffer->IsBufferReady())) {
            Logger::log("Cannot Draw Data! Buffers are not Ready.\n");
            return;
        }

        bd->cmdBuf->BeginRecording(); // start recording our commands to the cmd buffer

        bd->cmdBuf->BindProgram(&bd->shaderProgram, nvn::ShaderStageBits::VERTEX |
                                                    nvn::ShaderStageBits::FRAGMENT); // bind main imgui shader

        bd->cmdBuf->BindUniformBuffer(nvn::ShaderStage::VERTEX, 0, *bd->uniformMemory,
                                      UBOSIZE); // bind uniform block ptr
        bd->cmdBuf->UpdateUniformBuffer(*bd->uniformMemory, UBOSIZE, 0, sizeof(projMatrix),
                                        &projMatrix); // add projection matrix data to uniform data

        setRenderStates(); // sets up the rest of the render state, required so that our shader properly gets drawn to the screen

        size_t vtxOffset = 0, idxOffset = 0;
        nvn::TextureHandle boundTextureHandle = 0;

        ImVec2 newRes = getBackendData()->viewportSize;
        bd->cmdBuf->SetViewport(0, 0, newRes.x, newRes.y);

        // load data into buffers, and process draw commands
        for (size_t i = 0; i < drawData->CmdListsCount; i++) {

            auto cmdList = drawData->CmdLists[i];

            // calc vertex and index buffer sizes
            size_t vtxSize = cmdList->VtxBuffer.Size * sizeof(ImDrawVert);
            size_t idxSize = cmdList->IdxBuffer.Size * sizeof(ImDrawIdx);

            // bind vtx buffer at the current offset
            bd->cmdBuf->BindVertexBuffer(0, (*bd->vtxBuffer) + vtxOffset, vtxSize);

            // copy data from imgui command list into our gpu dedicated memory
            memcpy(bd->vtxBuffer->GetMemPtr() + vtxOffset, cmdList->VtxBuffer.Data, vtxSize);
            memcpy(bd->idxBuffer->GetMemPtr() + idxOffset, cmdList->IdxBuffer.Data, idxSize);

            for (auto cmd: cmdList->CmdBuffer) {

                // im not exactly sure this scaling is a good solution,
                // for some reason imgui clipping coords are relative to 720p instead of whatever I set for disp size.
                // bd->cmdBuf->SetScissor(cmd.ClipRect.x, cmd.ClipRect.y, newRes.x - cmd.ClipRect.x, newRes.y - cmd.ClipRect.y);
                if (newRes.y == 1080) {
                    bd->cmdBuf->SetScissor(cmd.ClipRect.x, cmd.ClipRect.y, cmd.ClipRect.z * 1.5 - cmd.ClipRect.x, cmd.ClipRect.w * 1.5 - cmd.ClipRect.y);
                } else {
                    bd->cmdBuf->SetScissor(cmd.ClipRect.x, cmd.ClipRect.y, cmd.ClipRect.z - cmd.ClipRect.x, cmd.ClipRect.w - cmd.ClipRect.y);
                }

                // get texture ID from the command
                nvn::TextureHandle TexID = *(nvn::TextureHandle *) cmd.GetTexID();
                // if our previous handle is different from the current, bind the texture
                if (boundTextureHandle != TexID) {
                    boundTextureHandle = TexID;
                    bd->cmdBuf->BindTexture(nvn::ShaderStage::FRAGMENT, 0, TexID);
                }
                // draw our vertices using the indices stored in the buffer, offset by the current command index offset,
                // as well as the current offset into our buffer.
                bd->cmdBuf->DrawElementsBaseVertex(nvn::DrawPrimitive::TRIANGLES,
                                                   nvn::IndexType::UNSIGNED_SHORT, cmd.ElemCount,
                                                   (*bd->idxBuffer) + (cmd.IdxOffset * sizeof(ImDrawIdx)) + idxOffset,
                                                   cmd.VtxOffset);
            }

            vtxOffset += vtxSize;
            idxOffset += idxSize;
        }

        // end the command recording and submit to queue.
        auto handle = bd->cmdBuf->EndRecording();
        bd->queue->SubmitCommands(1, &handle);
    }
}
