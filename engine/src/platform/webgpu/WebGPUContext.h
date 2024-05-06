#pragma once

#include "GfxContext.h"
#include "RenderTarget.h"
#include "WebGPUTexture.h"
#include <webgpu/webgpu.h>

namespace Figment
{
    class WebGPUContext : public GfxContext
    {
    public:
        void Init() override;
        void SwapBuffers() override;

        void Init(uint32_t width, uint32_t height);
        WGPUDevice GetDevice() { return m_WebGPUDevice; }
        WGPUSurface GetSurface() { return m_WebGPUSurface; }
        WGPUSwapChain GetSwapChain() { return m_SwapChain; }
        WGPUTextureFormat GetTextureFormat() { return m_TextureFormat; }
        uint32_t GetSwapChainWidth() { return m_SwapChainWidth; }
        uint32_t GetSwapChainHeight() { return m_SwapChainHeight; }
        RenderTarget GetDefaultRenderTarget() { return m_DefaultRenderTarget; }
        void CreateSwapChain(uint32_t width, uint32_t height);
    private:
        void CreateDefaultRenderTarget();

        uint32_t m_SwapChainWidth;
        uint32_t m_SwapChainHeight;
        WGPUTextureFormat m_TextureFormat;
        WGPUDevice m_WebGPUDevice;
        WGPUSurface m_WebGPUSurface;
        WGPUSwapChain m_SwapChain;
        WGPUInstance m_Instance;
        WebGPUTexture *m_DepthTexture;
        RenderTarget m_DefaultRenderTarget;
    };
}
