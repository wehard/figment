#pragma once

#include <vulkan/vulkan.h>

namespace figment::vulkan
{
struct BufferDescriptor
{
    const char* debugName     = nullptr;
    size_t byteSize           = 0;
    VkBufferUsageFlags usage  = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    uint32_t memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    bool map                  = false;
};

struct Buffer
{
    VkBuffer vkBuffer       = VK_NULL_HANDLE;
    VkDeviceMemory vkMemory = VK_NULL_HANDLE;
    uint32_t byteSize       = 0;
    void* hostPtr           = nullptr;
};

Buffer createBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                    const BufferDescriptor&& descriptor);
void destroyBuffer(const VkDevice& device, const Buffer& buffer);
} // namespace figment::vulkan
