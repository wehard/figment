#pragma once

#include "Arena.h"
#include "Handle.h"
#include "Pool.h"
#include "VulkanRenderPass.h"
#include "buffer.h"
#include "context.h"

namespace figment::vulkan
{

class ResourceManager
{
public:
    struct PoolSizes
    {
        uint32_t Buffers      = 128;
        uint32_t RenderPasses = 128;
    };

    explicit ResourceManager(const Context& context, const PoolSizes& poolSizes):
        m_Context(context),
        m_Buffers(poolSizes.Buffers, [](Buffer* item) { delete item; }),
        m_RenderPasses(poolSizes.RenderPasses, [](VulkanRenderPass* item) { delete item; }){};
    ~ResourceManager() = default;

    Handle<Buffer> CreateBuffer(const BufferDescriptor&& descriptor)
    {
        // return m_Buffers.Create(m_Context, std::move(descriptor));
        return Handle<Buffer>();
    }

    Handle<VulkanRenderPass>
    CreateRenderPass(const VulkanRenderPass::RenderPassDescriptor&& descriptor)
    {
        // return m_RenderPasses.Create(m_Context, std::move(descriptor));
        return Handle<VulkanRenderPass>();
    }

    Buffer* GetBuffer(Handle<Buffer> handle) { return m_Buffers.Get(handle); }
    VulkanRenderPass* GetRenderPass(Handle<VulkanRenderPass> handle)
    {
        return m_RenderPasses.Get(handle);
    }

private:
    const Context& m_Context;

    Pool<Buffer> m_Buffers;
    Pool<VulkanRenderPass> m_RenderPasses;
};
} // namespace figment::vulkan
