#include "VulkanBindGroup.h"
#include "VulkanBuffer.h"

namespace figment
{
VulkanBindGroup::VulkanBindGroup(const VulkanContext& context,
                                 const BindGroupDescriptor& descriptor):
    m_Context(context)
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    for (const auto& binding: descriptor.Bindings)
    {
        layoutBindings.emplace_back(binding.DescriptorSetLayoutBinding);
    }
    VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutCreateInfo.bindingCount = layoutBindings.size();
    layoutCreateInfo.pBindings    = layoutBindings.data();

    VkResult res = vkCreateDescriptorSetLayout(m_Context.GetDevice(), &layoutCreateInfo, nullptr,
                                               &m_DescriptorSetLayout);
    if (res != VK_SUCCESS)
        throw std::runtime_error("Failed to create descriptor set layout!");

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
    descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool     = descriptor.DescriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts        = &m_DescriptorSetLayout;

    checkVkResult(vkAllocateDescriptorSets(m_Context.GetDevice(), &descriptorSetAllocateInfo,
                                           &m_DescriptorSet));

    for (int i = 0; i < descriptor.Bindings.size(); i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer                    = descriptor.Bindings[i].Buffer->Get();
        bufferInfo.offset                    = 0;
        bufferInfo.range                     = descriptor.Bindings[i].Buffer->ByteSize();

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet               = m_DescriptorSet;
        descriptorWrite.dstBinding      = descriptor.Bindings[i].DescriptorSetLayoutBinding.binding;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType =
            descriptor.Bindings[i].DescriptorSetLayoutBinding.descriptorType;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pBufferInfo      = &bufferInfo;
        descriptorWrite.pImageInfo       = nullptr;
        descriptorWrite.pTexelBufferView = nullptr;

        vkUpdateDescriptorSets(m_Context.GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

VulkanBindGroup::~VulkanBindGroup()
{
    // TODO: Implement destructor
}
} // namespace Figment
