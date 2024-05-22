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
        };
        
        explicit VulkanPipeline(const VulkanContext &context, const VulkanShader &shader);
        ~VulkanPipeline();
    private:
        void CreatePipeline(const PipelineDescriptor &&descriptor);

        const VulkanContext &m_Context;
        const VulkanShader &m_Shader;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
    };

}
