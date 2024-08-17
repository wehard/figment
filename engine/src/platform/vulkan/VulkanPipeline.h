#pragma once
#include "VulkanShader.h"

namespace Figment
{
    class VulkanPipeline
    {
    public:
        struct VertexInputDescriptor
        {
            uint32_t Binding = 0;
            uint32_t Stride = 0;
            uint32_t InputRate = 0;
            std::vector<VkVertexInputAttributeDescription> Attributes;
        };

        struct PipelineDescriptor
        {
            uint32_t ViewportWidth = 0;
            uint32_t ViewportHeight = 0;
            VertexInputDescriptor VertexInput;
            VkRenderPass RenderPass = VK_NULL_HANDLE;
            VkShaderModule VertexModule = VK_NULL_HANDLE;
            VkShaderModule FragmentModule = VK_NULL_HANDLE;
            std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayoutBindings;
            VkShaderStageFlags PushConstantStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            uint32_t PushConstantOffset = 0;
            uint32_t PushConstantSize = 0;
        };

        explicit VulkanPipeline(const VulkanContext &context, const PipelineDescriptor &&descriptor);
        [[nodiscard]] VkPipeline Get() const { return m_Pipeline; }
        ~VulkanPipeline();
        [[nodiscard]] VkDescriptorSetLayout const *GetDescriptorSetLayout() { return &m_DescriptorSetLayout; }
        VkPipelineLayout GetLayout() const { return m_PipelineLayout; }
    private:
        void CreatePipeline(const PipelineDescriptor &descriptor);

        const VulkanContext &m_Context;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
    };
}
