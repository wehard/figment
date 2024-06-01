#pragma once

#include "VulkanContext.h"

namespace Figment
{
    class VulkanBindGroup
    {
    public:
        struct VulkanBufferInfo
        {
            VulkanBuffer *Buffer;
        };

        struct BindingDescriptor
        {
            VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding;
            VulkanBuffer *Buffer;
        };

        struct BindGroupDescriptor
        {
            VkDescriptorPool DescriptorPool;
            std::vector<BindingDescriptor> Bindings;
        };
    public:
        VulkanBindGroup(const VulkanContext &context, const BindGroupDescriptor &descriptor);
        ~VulkanBindGroup();
        [[nodiscard]] VkDescriptorSet *Get() { return &m_DescriptorSet; }
        [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout; }
    private:
        const VulkanContext &m_Context;
        VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    };

}
