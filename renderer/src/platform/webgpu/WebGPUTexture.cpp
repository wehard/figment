#include "WebGPUTexture.h"

WebGPUTexture::WebGPUTexture(WGPUDevice device, WGPUTextureFormat textureFormat, uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height), m_TextureFormat(textureFormat)
{
    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.nextInChain = nullptr;
    textureDescriptor.dimension = WGPUTextureDimension_2D;
    textureDescriptor.format = textureFormat;
    textureDescriptor.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc;
    textureDescriptor.size = { m_Width, m_Height, 1 };
    textureDescriptor.sampleCount = 1;
    textureDescriptor.mipLevelCount = 1;
    m_Texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

    WGPUTextureViewDescriptor textureViewDesc = {};
    textureViewDesc.nextInChain = nullptr;
    textureViewDesc.aspect = WGPUTextureAspect_All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = WGPUTextureViewDimension_2D;
    textureViewDesc.format = textureDescriptor.format;

    m_TextureView = wgpuTextureCreateView(m_Texture, &textureViewDesc);

    WGPUSamplerDescriptor samplerDesc = {};
    samplerDesc.nextInChain = nullptr;

    m_Sampler = wgpuDeviceCreateSampler(device, &samplerDesc);
}

WebGPUTexture::~WebGPUTexture()
{
    wgpuTextureViewRelease(m_TextureView);
    wgpuTextureRelease(m_Texture);
    wgpuTextureDestroy(m_Texture);
}

uint32_t WebGPUTexture::GetPixel(int x, int y)
{
    return 0;
}
