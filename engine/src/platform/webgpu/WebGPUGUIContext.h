#pragma once

#include <emscripten/html5_webgpu.h>
#include "GUIContext.h"
#include "WebGPUContext.h"

namespace Figment
{
    class WebGPUGUIContext : public GUIContext
    {
    private:
        ImGuiContext *m_Context;
        WebGPUContext *m_GfxContext;

    public:
        WebGPUGUIContext() = default;
        void Init(std::shared_ptr<Window> window, const char *glslVersion) override;
        void Begin();
        void Render() override;
        void Shutdown() override;
        ~WebGPUGUIContext() override = default;
    };
}
