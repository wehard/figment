#pragma once

#include "VulkanContext.h"
#include "vulkan/vulkan.h"

namespace Figment
{

    class VulkanBuffer
    {
    public:
        VulkanBuffer(VulkanContext *context, void *bufferData, uint32_t byteSize);
        ~VulkanBuffer() = default;
        VkBuffer Get() { return m_Buffer; }
        uint32_t ByteSize() { return m_ByteSize; }
    private:
        VkBuffer m_Buffer;
        VkDeviceMemory m_BufferMemory;
        uint32_t m_ByteSize;
    };

} // Figment
