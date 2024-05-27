#pragma once

#include "VulkanContext.h"
#include "vulkan/vulkan.h"

namespace Figment
{
    struct VulkanBufferDescriptor
    {
        void *Data = nullptr;
        uint32_t ByteSize = 0;
        VkBufferUsageFlags Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    };

    class VulkanBuffer
    {
    public:
        VulkanBuffer(VulkanContext *context, const VulkanBufferDescriptor &&descriptor);
        ~VulkanBuffer() = default;
        VkBuffer Get() { return m_Buffer; }
        [[nodiscard]] uint32_t ByteSize() const { return m_ByteSize; }
    private:
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
        uint32_t m_ByteSize = 0;
    };

} // Figment
