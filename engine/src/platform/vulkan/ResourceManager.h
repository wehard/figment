#pragma once

#include "Arena.h"
#include "Handle.h"
#include "Pool.h"
#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"

namespace figment
{

class ResourceManager
{
public:
    struct PoolSizes
    {
        uint32_t Buffers      = 128;
        uint32_t RenderPasses = 128;
    };

    explicit ResourceManager(const VulkanContext& context, const PoolSizes& poolSizes):
        m_Context(context),
        m_Buffers(poolSizes.Buffers, [](VulkanBuffer* item) { delete item; }),
        m_RenderPasses(poolSizes.RenderPasses, [](VulkanRenderPass* item) { delete item; }){};
    ~ResourceManager() = default;

    Handle<VulkanBuffer> CreateBuffer(const VulkanBufferDescriptor&& descriptor)
    {
        return m_Buffers.Create(m_Context, std::move(descriptor));
    }

    Handle<VulkanRenderPass>
    CreateRenderPass(const VulkanRenderPass::RenderPassDescriptor&& descriptor)
    {
        return m_RenderPasses.Create(m_Context, std::move(descriptor));
    }

    VulkanBuffer* GetBuffer(Handle<VulkanBuffer> handle) { return m_Buffers.Get(handle); }
    VulkanRenderPass* GetRenderPass(Handle<VulkanRenderPass> handle)
    {
        return m_RenderPasses.Get(handle);
    }

private:
    const VulkanContext& m_Context;

    Pool<VulkanBuffer> m_Buffers;
    Pool<VulkanRenderPass> m_RenderPasses;
};
} // namespace figment
