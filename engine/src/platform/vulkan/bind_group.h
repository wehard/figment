#pragma once

#include "context.h"

#include "buffer.h"

namespace figment::vulkan
{
class BindGroup
{
public:
    struct VulkanBufferInfo
    {
        vulkan::Buffer* Buffer;
    };

    struct BindingDescriptor
    {
        VkDescriptorSetLayoutBinding DescriptorSetLayoutBinding;
        vulkan::Buffer* Buffer;
    };

    struct BindGroupDescriptor
    {
        VkDescriptorPool DescriptorPool;
        std::vector<BindingDescriptor> Bindings;
    };

public:
    BindGroup(const Context& context, const BindGroupDescriptor& descriptor);
    ~BindGroup();
    [[nodiscard]] VkDescriptorSet* Get() { return &m_DescriptorSet; }
    [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return m_DescriptorSetLayout; }

private:
    const Context& m_Context;
    VkDescriptorSet m_DescriptorSet             = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
};

} // namespace figment::vulkan
