#pragma once

#include "GfxContext.h"
#include <webgpu/webgpu_cpp.h>

class WebGPUContext : public GfxContext
{
public:
    void Init() override;
    void SwapBuffers() override;

    WGPUDevice GetDevice() { return m_WebGPUDevice; }
    WGPUSurface GetSurface() { return m_WebGPUSurface; }
private:
    WGPUDevice m_WebGPUDevice;
    WGPUSurface m_WebGPUSurface;
};
