#pragma once

#include "VulkanContext.h"
#include "VulkanBuffer.h"
#include "Pool.h"
#include "Handle.h"
#include "Arena.h"

namespace Figment
{

    class ResourceManager
    {
    public:
        struct PoolSizes
        {
            uint32_t Buffers = 128;
        };
        
        explicit ResourceManager(const VulkanContext &context, const PoolSizes &poolSizes) : m_Context(context)
        {
            m_Buffers = Pool<VulkanBuffer>(poolSizes.Buffers);
        };
        ~ResourceManager() = default;

        Handle<VulkanBuffer> CreateBuffer(const VulkanBufferDescriptor &&descriptor)
        {
            return m_Buffers.Create(m_Context, std::move(descriptor));
        }

        VulkanBuffer *GetBuffer(Handle<VulkanBuffer> handle) { return m_Buffers.Get(handle); }

    private:
        const VulkanContext &m_Context;

        Pool<VulkanBuffer> m_Buffers;
    };
}
