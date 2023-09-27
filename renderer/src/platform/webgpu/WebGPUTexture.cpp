#include "WebGPUTexture.h"

WebGPUTexture::WebGPUTexture(WGPUDevice device, WGPUTextureFormat textureFormat, uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height), m_TextureFormat(textureFormat)
{
    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.nextInChain = nullptr;
    textureDescriptor.label = "RenderTexture";
    textureDescriptor.dimension = WGPUTextureDimension_2D;
    textureDescriptor.format = textureFormat;
    textureDescriptor.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc;
    textureDescriptor.size = { m_Width, m_Height, 1 };
    textureDescriptor.sampleCount = 1;
    textureDescriptor.mipLevelCount = 1;
    textureDescriptor.viewFormatCount = 0;
    textureDescriptor.viewFormats = nullptr;
    m_Texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

    WGPUTextureViewDescriptor textureViewDesc = {};
    textureViewDesc.nextInChain = nullptr;
    textureViewDesc.label = "RenderTextureView";
    textureViewDesc.aspect = WGPUTextureAspect_All;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = WGPUTextureViewDimension_2D;
    textureViewDesc.format = textureDescriptor.format;
    m_TextureView = wgpuTextureCreateView(m_Texture, &textureViewDesc);
}

WebGPUTexture::~WebGPUTexture()
{
    wgpuTextureViewRelease(m_TextureView);
    wgpuTextureDestroy(m_Texture);
    wgpuTextureRelease(m_Texture);
}
WebGPUTexture *WebGPUTexture::CreateDepthTexture(WGPUDevice device, WGPUTextureFormat depthTextureFormat, uint32_t width, uint32_t height)
{
    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.nextInChain = nullptr;
    textureDescriptor.label = "DepthTexture";
    textureDescriptor.dimension = WGPUTextureDimension_2D;
    textureDescriptor.format = depthTextureFormat;
    textureDescriptor.usage = WGPUTextureUsage_RenderAttachment;
    textureDescriptor.size = { width, height, 1 };
    textureDescriptor.sampleCount = 1;
    textureDescriptor.mipLevelCount = 1;
    textureDescriptor.viewFormatCount = 1;
    textureDescriptor.viewFormats = &depthTextureFormat;
    WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

    WGPUTextureViewDescriptor textureViewDesc = {};
    textureViewDesc.nextInChain = nullptr;
    textureViewDesc.label = "DepthTextureView";
    textureViewDesc.aspect = WGPUTextureAspect_DepthOnly;
    textureViewDesc.baseArrayLayer = 0;
    textureViewDesc.arrayLayerCount = 1;
    textureViewDesc.baseMipLevel = 0;
    textureViewDesc.mipLevelCount = 1;
    textureViewDesc.dimension = WGPUTextureViewDimension_2D;
    textureViewDesc.format = depthTextureFormat;
    WGPUTextureView textureView = wgpuTextureCreateView(texture, &textureViewDesc);

    auto *depthTexture = new WebGPUTexture();
    depthTexture->m_Texture = texture;
    depthTexture->m_TextureView = textureView;
    depthTexture->m_Width = width;
    depthTexture->m_Height = height;
    depthTexture->m_TextureFormat = depthTextureFormat;
    return depthTexture;
}
