#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
struct DescriptorSetLayoutDescriptor
{
    std::vector<VkDescriptorSetLayoutBinding> bindings = {};
    uint32_t flags                                     = 0;
};

struct PipelineLayoutDescriptor
{
    std::vector<DescriptorSetLayoutDescriptor> descriptorSetLayoutDescriptors = {};
    VkShaderStageFlags pushConstantStageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uint32_t pushConstantOffset               = 0;
    uint32_t pushConstantSize                 = 0;
};

struct VertexInputAttributeDescriptor
{
    uint32_t location = 0;
    VkFormat format   = VK_FORMAT_UNDEFINED;
    uint32_t offset   = 0;
};

struct VertexInputDescriptor
{
    uint32_t binding                                       = 0;
    uint32_t stride                                        = 0;
    VkVertexInputRate inputRate                            = VK_VERTEX_INPUT_RATE_VERTEX;
    std::vector<VertexInputAttributeDescriptor> attributes = {};
};

struct PipelineDescriptor
{
    std::vector<VertexInputDescriptor> vertexInputs = {};
    VkFrontFace frontFace                           = VK_FRONT_FACE_CLOCKWISE;
    VkSampleCountFlagBits msaaSamples               = VK_SAMPLE_COUNT_1_BIT;
    VkFormat colorFormat                            = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat                            = VK_FORMAT_D32_SFLOAT;
    bool blendEnable                                = VK_TRUE;
};

struct PipelineLayout
{
    VkPipelineLayout vkLayout                     = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayout> setLayouts = {};
};

struct Pipeline
{
    PipelineLayout layout         = {};
    VkPipeline vkPipeline         = VK_NULL_HANDLE;
    VkShaderModule vertexModule   = VK_NULL_HANDLE;
    VkShaderModule fragmentModule = VK_NULL_HANDLE;
};

PipelineLayout createPipelineLayout(const VkDevice& device,
                                    const PipelineLayoutDescriptor&& descriptor);

VkPipeline createDynamicRenderingPipeline(const VkDevice& device,
                                          const PipelineDescriptor&& descriptor,
                                          const VkPipelineLayout& layout,
                                          const VkShaderModule& vertexModule,
                                          const VkShaderModule& fragmentModule);

Pipeline createPipeline(const VkDevice& device, const PipelineLayoutDescriptor&& layoutDescriptor,
                        const PipelineDescriptor&& pipelineDescriptor,
                        const VkShaderModule&& vertexModule, const VkShaderModule&& fragmentModule);

void destroyPipeline(const VkDevice& device, const Pipeline& pipeline);
} // namespace figment::vulkan
