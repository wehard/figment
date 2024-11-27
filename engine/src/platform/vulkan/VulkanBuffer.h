#pragma once

#include "VulkanContext.h"
#include "vulkan/vulkan.h"

#include <string>

namespace figment
{
struct VulkanBufferDescriptor
{
    const std::string& Name  = "VulkanBuffer";
    void* Data               = nullptr;
    size_t ByteSize          = 0;
    VkBufferUsageFlags Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    uint32_t MemoryProperties =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
};

class VulkanBuffer
{
public:
    VulkanBuffer(const VulkanContext& context, const VulkanBufferDescriptor&& descriptor);
    ~VulkanBuffer();
    void SetData(void* data, size_t byteSize);
    VkBuffer Get() { return m_Buffer; }
    [[nodiscard]] uint32_t ByteSize() const { return m_ByteSize; }
    [[nodiscard]] void* Map() const;

private:
    const VulkanContext& m_Context;
    std::string m_Name;
    VkBuffer m_Buffer             = VK_NULL_HANDLE;
    VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
    uint32_t m_ByteSize           = 0;
};

} // namespace figment
