#pragma once

#include <emscripten/html5_webgpu.h>
#include "GUIContext.h"
#include "WebGPUContext.h"

class WebGPUGUIContext : public GUIContext
{
private:
    ImGuiContext *m_Context;
    WebGPUContext *m_GfxContext;

public:
    WebGPUGUIContext() = default;
    void Init(std::shared_ptr<Window> window, const char *glslVersion) override;
    void Render() override {}
    static void Render(WGPURenderPassEncoder pass);
    void Shutdown() override;
    ~WebGPUGUIContext() override = default;
};
