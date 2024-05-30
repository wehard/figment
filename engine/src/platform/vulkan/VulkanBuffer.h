#pragma once

#include "VulkanContext.h"
#include "vulkan/vulkan.h"

namespace Figment
{
    struct VulkanBufferDescriptor
    {
        const char *Name = "VulkanBuffer";
        void *Data = nullptr;
        size_t ByteSize = 0;
        VkBufferUsageFlags Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        uint32_t MemoryProperties =
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    };

    class VulkanBuffer
    {
    public:
        VulkanBuffer(VulkanContext *context, const VulkanBufferDescriptor &&descriptor);
        ~VulkanBuffer();
        void SetData(void *data, size_t byteSize);
        VkBuffer Get() { return m_Buffer; }
        [[nodiscard]] uint32_t ByteSize() const { return m_ByteSize; }
    private:
        VulkanContext *m_Context;
        VkBuffer m_Buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
        uint32_t m_ByteSize = 0;
    };

} // Figment
