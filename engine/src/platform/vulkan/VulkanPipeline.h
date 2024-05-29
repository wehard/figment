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
        };

        explicit VulkanPipeline(const VulkanContext &context, const PipelineDescriptor &&descriptor);
        [[nodiscard]] VkPipeline Get() const { return m_Pipeline; }
        ~VulkanPipeline();
    private:
        void CreatePipeline(const PipelineDescriptor &descriptor);

        const VulkanContext &m_Context;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
    };
}
