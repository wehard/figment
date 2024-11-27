#include "buffer.h"
#include "context.h"
#include "utils.h"

namespace figment::vulkan
{
static uint32_t findMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes,
                                    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((allowedTypes & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    throw std::runtime_error("Failed to find required memory type!");
}

Buffer createBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                    const BufferDescriptor&& descriptor)
{
    Buffer buffer                 = {};

    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size               = descriptor.byteSize;
    bufferInfo.usage              = descriptor.usage;
    bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    checkVkResult(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer.vkBuffer));

    VkMemoryRequirements memoryRequirements = {};
    vkGetBufferMemoryRequirements(device, buffer.vkBuffer, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize       = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex      = findMemoryTypeIndex(
        physicalDevice, memoryRequirements.memoryTypeBits, descriptor.memoryProperties);

    checkVkResult(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &buffer.vkMemory));
    buffer.byteSize = descriptor.byteSize;

    vkBindBufferMemory(device, buffer.vkBuffer, buffer.vkMemory, 0);

    if (descriptor.map)
        vkMapMemory(device, buffer.vkMemory, 0, descriptor.byteSize, 0, &buffer.hostPtr);

    return buffer;
}

void destroyBuffer(const VkDevice& device, const Buffer& buffer)
{
    vkDestroyBuffer(device, buffer.vkBuffer, nullptr);
    vkFreeMemory(device, buffer.vkMemory, nullptr);
}
} // namespace figment::vulkan
