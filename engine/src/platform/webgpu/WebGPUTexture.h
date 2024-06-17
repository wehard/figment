#pragma once

#include <webgpu/webgpu.h>
#include "Image.h"

namespace Figment
{
    struct WebGPUTextureDescriptor
    {
        WGPUTextureFormat Format = WGPUTextureFormat_RGBA8Unorm;
        uint32_t Width = 0;
        uint32_t Height = 0;
        WGPUTextureUsageFlags Usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
        const std::string Label = "WebGPUTexture";
        WGPUTextureAspect Aspect = WGPUTextureAspect_All;
    };

    class WebGPUTexture
    {
    public:
        WebGPUTexture() = default;
        WebGPUTexture(WGPUDevice device, const WebGPUTextureDescriptor &&descriptor);
        ~WebGPUTexture();

        void SetData(const void *data, uint32_t size);
        WGPUTexture GetTexture() { return m_Texture; }
        WGPUTextureView GetTextureView() { return m_TextureView; }
        WGPUSampler GetSampler();
        WGPUTextureFormat GetTextureFormat() { return m_TextureFormat; }

        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }

    public:
        static WebGPUTexture *CreateDepthTexture(WGPUDevice device, WGPUTextureFormat depthTextureFormat,
                uint32_t width, uint32_t height);
        static WebGPUTexture *Create(WGPUDevice device, Image &image);

    private:
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
        WGPUDevice m_Device = nullptr;
        WGPUTextureFormat m_TextureFormat = WGPUTextureFormat_Undefined;
        WGPUTexture m_Texture = nullptr;
        WGPUTextureView m_TextureView = nullptr;
        WGPUSampler m_Sampler = nullptr;
    };
}
