#include "VulkanBuffer.h"
#include <cstring>
#include <spdlog/spdlog.h>

namespace figment
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

    VulkanBuffer::VulkanBuffer(const VulkanContext &context, const VulkanBufferDescriptor &&descriptor) :
            m_Context(context),
            m_Name(descriptor.Name),
            m_ByteSize(descriptor.ByteSize)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = descriptor.ByteSize;
        bufferInfo.usage = descriptor.Usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(context.GetDevice(), &bufferInfo, nullptr, &m_Buffer);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create vertex buffer!");

        // buffer memory requirements
        VkMemoryRequirements memoryRequirements = {};
        vkGetBufferMemoryRequirements(context.GetDevice(), m_Buffer, &memoryRequirements);

        // allocate memory to buffer
        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = FindMemoryTypeIndex(context.GetPhysicalDevice(),
                memoryRequirements.memoryTypeBits,
                descriptor.MemoryProperties);

        // allocate memory to device memory
        result = vkAllocateMemory(context.GetDevice(), &memoryAllocateInfo, nullptr, &m_BufferMemory);
        if (result != VK_SUCCESS)
            std::runtime_error("Failed to allocate vertex buffer memory!");

        // allocate meory to given vertex buffer
        vkBindBufferMemory(context.GetDevice(), m_Buffer, m_BufferMemory, 0);

        // map memory to vertex vuffer
        void *data;
        if (descriptor.Data != nullptr)
        {
            vkMapMemory(context.GetDevice(), m_BufferMemory, 0, bufferInfo.size, 0, &data);
            memcpy(data, descriptor.Data, (size_t)bufferInfo.size);
            vkUnmapMemory(context.GetDevice(), m_BufferMemory);
        }

        spdlog::info("{} created: size = {}", descriptor.Name, descriptor.ByteSize);
    }

    void VulkanBuffer::SetData(void *data, size_t byteSize)
    {
        if (byteSize > m_ByteSize)
            throw std::runtime_error("Data size exceeds buffer size!");
        void *dst;
        auto result = vkMapMemory(m_Context.GetDevice(), m_BufferMemory, 0, m_ByteSize, 0, &dst);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to map buffer memory!");
        memcpy(dst, data, byteSize);
        vkUnmapMemory(m_Context.GetDevice(), m_BufferMemory);
        // FIG_LOG_INFO("VulkanBuffer data updated: size = %d", byteSize);
    }

    VulkanBuffer::~VulkanBuffer()
    {
        vkDestroyBuffer(m_Context.GetDevice(), m_Buffer, nullptr);
        vkFreeMemory(m_Context.GetDevice(), m_BufferMemory, nullptr);
    }

    void *VulkanBuffer::Map() const
    {
        void *data;
        auto result = vkMapMemory(m_Context.GetDevice(), m_BufferMemory, 0, m_ByteSize, 0, &data);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to map buffer memory!");
        return data;
    }
}
