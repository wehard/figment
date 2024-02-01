#include "WebGPUTexture.h"

namespace Figment
{
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
        wgpuSamplerRelease(m_Sampler);
        wgpuTextureDestroy(m_Texture);
        wgpuTextureRelease(m_Texture);
    }
    WebGPUTexture *WebGPUTexture::CreateDepthTexture(WGPUDevice device, WGPUTextureFormat depthTextureFormat,
            uint32_t width, uint32_t height)
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

    WebGPUTexture *WebGPUTexture::Create(WGPUDevice device, Image &image)
    {
        WGPUTextureDescriptor textureDescriptor = {};
        textureDescriptor.nextInChain = nullptr;
        textureDescriptor.label = "Texture";
        textureDescriptor.dimension = WGPUTextureDimension_2D;
        textureDescriptor.format = WGPUTextureFormat_RGBA8Unorm;
        textureDescriptor.usage = WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding;
        textureDescriptor.size = { image.GetWidth(), image.GetHeight(), 1 };
        textureDescriptor.sampleCount = 1;
        textureDescriptor.mipLevelCount = 1;
        textureDescriptor.viewFormatCount = 0;
        textureDescriptor.viewFormats = nullptr;
        WGPUTexture texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

        WGPUTextureViewDescriptor textureViewDesc = {};
        textureViewDesc.nextInChain = nullptr;
        textureViewDesc.label = "TextureView";
        textureViewDesc.aspect = WGPUTextureAspect_All;
        textureViewDesc.baseArrayLayer = 0;
        textureViewDesc.arrayLayerCount = 1;
        textureViewDesc.baseMipLevel = 0;
        textureViewDesc.mipLevelCount = 1;
        textureViewDesc.dimension = WGPUTextureViewDimension_2D;
        textureViewDesc.format = textureDescriptor.format;
        WGPUTextureView textureView = wgpuTextureCreateView(texture, &textureViewDesc);

        WGPUSamplerDescriptor samplerDesc = {};
        samplerDesc.nextInChain = nullptr;
        samplerDesc.label = "Sampler";
        samplerDesc.addressModeU = WGPUAddressMode_Repeat;
        samplerDesc.addressModeV = WGPUAddressMode_Repeat;
        samplerDesc.addressModeW = WGPUAddressMode_Repeat;
        samplerDesc.magFilter = WGPUFilterMode_Nearest;
        samplerDesc.minFilter = WGPUFilterMode_Nearest;
        samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Nearest;
        samplerDesc.lodMinClamp = 0;
        samplerDesc.lodMaxClamp = 4;
        samplerDesc.compare = WGPUCompareFunction_Undefined;
        samplerDesc.maxAnisotropy = 1;
        WGPUSampler sampler = wgpuDeviceCreateSampler(device, &samplerDesc);

        auto *webGpuTexture = new WebGPUTexture();
        webGpuTexture->m_Texture = texture;
        webGpuTexture->m_TextureView = textureView;
        webGpuTexture->m_Sampler = sampler;
        webGpuTexture->m_Width = image.GetWidth();
        webGpuTexture->m_Height = image.GetHeight();
        webGpuTexture->m_TextureFormat = textureDescriptor.format;

        WGPUQueue queue = wgpuDeviceGetQueue(device);
        WGPUImageCopyTexture imageCopyTexture = {};
        imageCopyTexture.texture = texture;
        imageCopyTexture.mipLevel = 0;
        imageCopyTexture.origin = { 0, 0, 0 };
        imageCopyTexture.aspect = WGPUTextureAspect_All;

        WGPUTextureDataLayout textureDataLayout = {};
        textureDataLayout.nextInChain = nullptr;
        textureDataLayout.offset = 0;
        textureDataLayout.bytesPerRow = image.GetWidth() * 4;
        textureDataLayout.rowsPerImage = image.GetHeight();

        wgpuQueueWriteTexture(queue, &imageCopyTexture, image.GetData(), image.GetDataSize(), &textureDataLayout,
                &textureDescriptor.size);
        return webGpuTexture;
    }
}
