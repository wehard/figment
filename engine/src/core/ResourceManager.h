#pragma once

#include "RenderContext.h"
#include "Pool.h"
#include "Handle.h"

namespace Figment
{
    struct TextureDescriptor
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
        uint32_t arrayLayerCount = 1;
        uint32_t mipLevelCount = 1;
        // uint32_t format = WGPUTextureFormat_RGBA8Unorm;
        // uint32_t usage = static_cast<WGPUTextureUsage>(WGPUTextureUsage_TextureBinding
        //         | WGPUTextureUsage_CopySrc);
    };

    struct Texture
    {
        uint32_t width = 0;
        uint32_t height = 0;
    };

    struct BindGroupDescriptor
    {
        uint32_t size = 0;
    };

    struct BindGroup
    {
        uint32_t size = 0;
    };

    class ResourceManager
    {
    public:
        explicit ResourceManager(RenderContext &context) : m_Context(context) { };
        ~ResourceManager() = default;

        Handle<Texture> CreateTexture(const TextureDescriptor &&descriptor);
        Handle<BindGroup> CreateBindGroup(const BindGroupDescriptor &&descriptor);

        Texture *GetTexture(Handle<Texture> handle) { return m_TextureHandles.Get(handle); }
        BindGroup *GetBindGroup(Handle<BindGroup> handle) { return m_BindGroupHandles.Get(handle); }

        [[nodiscard]] uint32_t ResourceCount() const { return m_TextureHandles.Count() + m_BindGroupHandles.Count(); }
        [[nodiscard]] uint32_t TextureCount() const { return m_TextureHandles.Count(); }
        [[nodiscard]] uint32_t BindGroupCount() const { return m_BindGroupHandles.Count(); }
    private:
        RenderContext &m_Context;

        Pool<Texture> m_TextureHandles;
        Pool<BindGroup> m_BindGroupHandles;
    };
}
