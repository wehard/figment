#pragma once

#include <webgpu/webgpu.h>
#include "Image.h"

namespace Figment
{
    class WebGPUTexture
    {
    public:
        WebGPUTexture() = default;
        WebGPUTexture(WGPUDevice device, WGPUTextureFormat textureFormat, uint32_t width, uint32_t height);
        WebGPUTexture(WGPUDevice device, WGPUTextureFormat textureFormat, uint32_t width, uint32_t height,
                WGPUTextureUsageFlags usage, const std::string &label = "WebGPUTexture");
        ~WebGPUTexture();

        void SetData(const void *data, uint32_t size);
        WGPUTexture GetTexture()
        { return m_Texture; }
        WGPUTextureView GetTextureView()
        { return m_TextureView; }
        WGPUSampler GetSampler();
        uint32_t GetWidth()
        { return m_Width; }
        uint32_t GetHeight()
        { return m_Height; }
        WGPUTextureFormat GetTextureFormat()
        { return m_TextureFormat; }
        static WebGPUTexture *CreateDepthTexture(WGPUDevice device, WGPUTextureFormat depthTextureFormat,
                uint32_t width, uint32_t height);
        static WebGPUTexture *Create(WGPUDevice device, Image &image);
    private:
        uint32_t m_Width;
        uint32_t m_Height;
        WGPUDevice m_Device;
        WGPUTextureFormat m_TextureFormat;
        WGPUTexture m_Texture;
        WGPUTextureView m_TextureView;
        WGPUSampler m_Sampler = nullptr;
    };
}
