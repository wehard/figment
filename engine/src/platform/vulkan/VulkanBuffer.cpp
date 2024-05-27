#include "VulkanBuffer.h"
#include "Log.h"

namespace Figment
{
    static uint32_t FindMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t allowedTypes,
            VkMemoryPropertyFlags properties)
    {
        // get properties of physical device memory
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        // find index for required memory type
        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to find required memory type!");
        return (-1);
    }

    VulkanBuffer::VulkanBuffer(VulkanContext *context, const VulkanBufferDescriptor &&descriptor) : m_ByteSize(
            descriptor.ByteSize)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = descriptor.ByteSize;
        bufferInfo.usage = descriptor.Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(context->GetDevice(), &bufferInfo, nullptr, &m_Buffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create vertex buffer!");

        // buffer memory requirements
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(context->GetDevice(), m_Buffer, &memoryRequirements);

        // allocate memory to buffer
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(context->GetPhysicalDevice(),
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // allocate memory to device memory
        result = vkAllocateMemory(context->GetDevice(), &memoryAllocateInfo, nullptr, &m_BufferMemory);
        if (result != VK_SUCCESS)
            std::runtime_error("Failed to allocate vertex buffer memory!");

        // allocate meory to given vertex buffer
        vkBindBufferMemory(context->GetDevice(), m_Buffer, m_BufferMemory, 0);

        // map memory to vertex vuffer
        void *data;
        vkMapMemory(context->GetDevice(), m_BufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, descriptor.Data, (size_t)bufferInfo.size);
        vkUnmapMemory(context->GetDevice(), m_BufferMemory);

        FIG_LOG_INFO("Vulkan buffer created: size = %d", descriptor.ByteSize);
    }
}
