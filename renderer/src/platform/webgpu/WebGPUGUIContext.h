#pragma once

#include <emscripten/html5_webgpu.h>
#include "GUIContext.h"
#include "WebGPUContext.h"

class WebGPUGUIContext : public GUIContext
{
private:
    ImGuiContext *m_Context;
    WGPUSwapChain m_SwapChain;
    WebGPUContext *m_GfxContext;

public:
    WebGPUGUIContext() = default;
    void Init(std::shared_ptr<Window> window, const char *glslVersion) override;
    void Render() override;
    void Shutdown() override;
    ~WebGPUGUIContext() override = default;
};