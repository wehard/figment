#include "WebGPUTexture.h"

WebGPUTexture::WebGPUTexture(WGPUDevice device, WGPUTextureFormat textureFormat, uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height), m_TextureFormat(textureFormat)
{
    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.nextInChain = nullptr;
    textureDescriptor.dimension = WGPUTextureDimension_2D;
    textureDescriptor.format = textureFormat;
    textureDescriptor.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment;
    textureDescriptor.size = { m_Width, m_Height, 1 };
    textureDescriptor.sampleCount = 1;
    textureDescriptor.mipLevelCount = 1;
    m_Texture = wgpuDeviceCreateTexture(device, &textureDescriptor);
    m_TextureView = wgpuTextureCreateView(m_Texture, nullptr);
}

WebGPUTexture::~WebGPUTexture()
{
    wgpuTextureViewRelease(m_TextureView);
    wgpuTextureRelease(m_Texture);
    wgpuTextureDestroy(m_Texture);
}
