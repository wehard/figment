#pragma once

#include "GfxContext.h"
#include <webgpu/webgpu_cpp.h>

class WebGPUContext : public GfxContext
{
public:
    void Init() override;
    void SwapBuffers() override;
private:
    WGPUDevice m_WebGPUDevice;
    WGPUSurface m_WebGPUSurface;
};
