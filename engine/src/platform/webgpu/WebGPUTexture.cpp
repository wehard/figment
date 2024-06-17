#include "WebGPUTexture.h"

namespace Figment
{
    WebGPUTexture::WebGPUTexture(WGPUDevice device, const WebGPUTextureDescriptor &&descriptor) : m_Width(
            descriptor.Width),
            m_Height(descriptor.Height), m_TextureFormat(descriptor.Format), m_Device(device)
    {
        WGPUTextureDescriptor textureDescriptor = {};
        textureDescriptor.nextInChain = nullptr;
        textureDescriptor.label = descriptor.Label.c_str();
        textureDescriptor.dimension = WGPUTextureDimension_2D;
        textureDescriptor.format = descriptor.Format;
        textureDescriptor.usage = descriptor.Usage;
        textureDescriptor.size = { descriptor.Width, descriptor.Height, 1 };
        textureDescriptor.sampleCount = 1;
        textureDescriptor.mipLevelCount = 1;
        textureDescriptor.viewFormatCount = 0;
        textureDescriptor.viewFormats = nullptr;
        m_Texture = wgpuDeviceCreateTexture(device, &textureDescriptor);

        WGPUTextureViewDescriptor textureViewDesc = {};
        textureViewDesc.nextInChain = nullptr;
        textureViewDesc.label = (descriptor.Label + "View").c_str();
        textureViewDesc.aspect = descriptor.Aspect;
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
        // TODO: This is causing a crash
        // wgpuSamplerRelease(m_Sampler);
        wgpuTextureDestroy(m_Texture);
        wgpuTextureRelease(m_Texture);
    }

    void WebGPUTexture::SetData(const void *data, uint32_t size)
    {
        WGPUQueue queue = wgpuDeviceGetQueue(m_Device);
        WGPUImageCopyTexture imageCopyTexture = {};
        imageCopyTexture.texture = m_Texture;
        imageCopyTexture.mipLevel = 0;
        imageCopyTexture.origin = { 0, 0, 0 };
        imageCopyTexture.aspect = WGPUTextureAspect_All;

        WGPUTextureDataLayout textureDataLayout = {};
        textureDataLayout.nextInChain = nullptr;
        textureDataLayout.offset = 0;
        textureDataLayout.bytesPerRow = m_Width * 4;
        textureDataLayout.rowsPerImage = m_Height;
        WGPUExtent3D extent = { m_Width, m_Height, 1 };

        wgpuQueueWriteTexture(queue, &imageCopyTexture, data, size, &textureDataLayout,
                &extent);
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

    WGPUSampler WebGPUTexture::GetSampler()
    {
        if (m_Sampler)
        {
            return m_Sampler;
        }

        WGPUSamplerDescriptor samplerDesc = {};
        samplerDesc.nextInChain = nullptr;
        samplerDesc.label = "WebGPUTextureSampler";
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
        m_Sampler = wgpuDeviceCreateSampler(m_Device, &samplerDesc);
        return m_Sampler;
    }
}
