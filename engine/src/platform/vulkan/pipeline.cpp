#include "pipeline.h"
#include "context.h"
#include "utils.h"

#include <array>
#include <memory>

namespace figment::vulkan
{

static VkDescriptorSetLayout
createDescriptorSetLayout(const VkDevice& device,
                          const DescriptorSetLayoutDescriptor& layoutDescriptor)
{
    const VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {
        .sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext        = nullptr,
        .flags        = layoutDescriptor.flags,
        .bindingCount = static_cast<uint32_t>(layoutDescriptor.bindings.size()),
        .pBindings    = layoutDescriptor.bindings.data(),
    };

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    checkVkResult(
        vkCreateDescriptorSetLayout(device, &layoutCreateInfo, nullptr, &descriptorSetLayout));
    return descriptorSetLayout;
}

PipelineLayout createPipelineLayout(const VkDevice& device,
                                    const PipelineLayoutDescriptor&& descriptor)
{
    PipelineLayout pipelineLayout = {};
    pipelineLayout.setLayouts.reserve(descriptor.descriptorSetLayoutDescriptors.size());

    for (const auto& layoutDescriptor: descriptor.descriptorSetLayoutDescriptors)
    {
        pipelineLayout.setLayouts.emplace_back(createDescriptorSetLayout(device, layoutDescriptor));
    }

    const VkPushConstantRange pushConstantRange = {
        .stageFlags = descriptor.pushConstantStageFlags,
        .offset     = descriptor.pushConstantOffset,
        .size       = descriptor.pushConstantSize,
    };

    const uint32_t pushConstantRangeCount = descriptor.pushConstantSize > 0 ? 1 : 0;

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .setLayoutCount         = static_cast<uint32_t>(pipelineLayout.setLayouts.size()),
        .pSetLayouts            = pipelineLayout.setLayouts.data(),
        .pushConstantRangeCount = pushConstantRangeCount,
        .pPushConstantRanges    = &pushConstantRange,
    };

    checkVkResult(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr,
                                         &pipelineLayout.vkLayout));
    return pipelineLayout;
}

struct PipelineVertexInputState
{
    VkPipelineVertexInputStateCreateInfo createInfo                      = {};
    std::vector<VkVertexInputBindingDescription> bindingDescriptions     = {};
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {};
};

static PipelineVertexInputState
createPipelineVertexInputState(const std::vector<VertexInputDescriptor>& vertexInputs)
{
    PipelineVertexInputState state;

    for (const auto& vertexInput: vertexInputs)
    {
        VkVertexInputBindingDescription bindingDescription = {
            .binding   = vertexInput.binding,
            .stride    = vertexInput.stride,
            .inputRate = vertexInput.inputRate,
        };
        state.bindingDescriptions.emplace_back(bindingDescription);

        for (const auto& attribute: vertexInput.attributes)
        {
            VkVertexInputAttributeDescription attributeDescription = {
                .location = attribute.location,
                .binding  = vertexInput.binding,
                .format   = attribute.format,
                .offset   = attribute.offset,
            };
            state.attributeDescriptions.emplace_back(attributeDescription);
        }
    }

    state.createInfo = VkPipelineVertexInputStateCreateInfo{
        .sType                         = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext                         = nullptr,
        .flags                         = 0,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(state.bindingDescriptions.size()),
        .pVertexBindingDescriptions    = state.bindingDescriptions.data(),
        .vertexAttributeDescriptionCount =
            static_cast<uint32_t>(state.attributeDescriptions.size()),
        .pVertexAttributeDescriptions = state.attributeDescriptions.data(),
    };

    return state;
}

static VkPipelineInputAssemblyStateCreateInfo createPipelineInputAssemblyState()
{
    return {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext                  = nullptr,
        .flags                  = 0,
        .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
}

static VkPipelineViewportStateCreateInfo createPipelineViewportState()
{
    return {
        .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext         = nullptr,
        .flags         = 0,
        .viewportCount = 1,
        .pViewports    = nullptr,
        .scissorCount  = 1,
        .pScissors     = nullptr,
    };
}

static VkPipelineRasterizationStateCreateInfo
createPipelineRasterizationState(const VkFrontFace frontFace)
{
    return {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext                   = nullptr,
        .flags                   = 0,
        .depthClampEnable        = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode             = VK_POLYGON_MODE_FILL,
        .cullMode                = VK_CULL_MODE_BACK_BIT,
        .frontFace               = frontFace,
        .depthBiasEnable         = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp          = 0.0f,
        .depthBiasSlopeFactor    = 0.0f,
        .lineWidth               = 1.0f,
    };
}

static VkPipelineMultisampleStateCreateInfo
createPipelineMultisampleState(const VkSampleCountFlagBits samples)
{
    return {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .rasterizationSamples  = samples,
        .sampleShadingEnable   = VK_TRUE,
        .minSampleShading      = 0.2f,
        .pSampleMask           = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable      = VK_FALSE,
    };
}

static VkPipelineColorBlendAttachmentState
createPipelineColorBlendAttachmentState(const bool blendEnable)
{
    return {
        .blendEnable         = blendEnable,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp        = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .alphaBlendOp        = VK_BLEND_OP_ADD,
        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
}

static VkPipelineColorBlendStateCreateInfo
createPipelineColorBlendState(const VkPipelineColorBlendAttachmentState* colorState)
{
    return {
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0,
        .logicOpEnable   = VK_FALSE,
        .logicOp         = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments    = colorState,
        .blendConstants  = {0.0f, 0.0f, 0.0f, 0.0f},
    };
}

static VkPipelineDepthStencilStateCreateInfo createPipelineDepthStencilState()
{
    return {
        .sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext                 = nullptr,
        .flags                 = 0,
        .depthTestEnable       = VK_TRUE,
        .depthWriteEnable      = VK_TRUE,
        .depthCompareOp        = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable     = VK_FALSE,
        .front                 = {},
        .back                  = {},
        .minDepthBounds        = 0.0f,
        .maxDepthBounds        = 1.0f,
    };
}

static std::array<VkPipelineShaderStageCreateInfo, 2>
createPipelineShaderStages(const VkShaderModule& vertexModule, const VkShaderModule& fragmentModule)
{
    const VkPipelineShaderStageCreateInfo vertexStage = {
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stage               = VK_SHADER_STAGE_VERTEX_BIT,
        .module              = vertexModule,
        .pName               = "main",
        .pSpecializationInfo = nullptr,
    };

    VkPipelineShaderStageCreateInfo fragmentStage = {
        .sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext               = nullptr,
        .flags               = 0,
        .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module              = fragmentModule,
        .pName               = "main",
        .pSpecializationInfo = nullptr,
    };

    return {vertexStage, fragmentStage};
}

struct PipelineDynamicState
{
    VkPipelineDynamicStateCreateInfo createInfo = {};
    std::array<VkDynamicState, 2> dynamicStates = {};
};

static PipelineDynamicState createPipelineDynamicState()
{
    PipelineDynamicState info;
    info.dynamicStates = std::array{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    info.createInfo    = {
           .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
           .pNext             = nullptr,
           .flags             = 0,
           .dynamicStateCount = static_cast<uint32_t>(info.dynamicStates.size()),
           .pDynamicStates    = info.dynamicStates.data(),
    };
    return info;
}

struct PipelineRenderingInfo
{
    std::array<VkFormat, 1> colorFormats        = {};
    VkPipelineRenderingCreateInfoKHR createInfo = {};
};

static PipelineRenderingInfo createPipelineRenderingInfo(const VkFormat colorFormat,
                                                         const VkFormat depthFormat)
{
    PipelineRenderingInfo info = {};
    info.colorFormats          = std::array{colorFormat};
    info.createInfo            = {
                   .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
                   .pNext                   = nullptr,
                   .viewMask                = 0,
                   .colorAttachmentCount    = static_cast<uint32_t>(info.colorFormats.size()),
                   .pColorAttachmentFormats = info.colorFormats.data(),
                   .depthAttachmentFormat   = depthFormat,
                   .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };
    return info;
}

VkPipeline createDynamicRenderingPipeline(const VkDevice& device,
                                          const PipelineDescriptor&& descriptor,
                                          const VkPipelineLayout& layout,
                                          const VkShaderModule& vertexModule,
                                          const VkShaderModule& fragmentModule)
{
    PipelineRenderingInfo pipelineRendering =
        createPipelineRenderingInfo(descriptor.colorFormat, descriptor.depthFormat);

    std::array shaderStages = createPipelineShaderStages(vertexModule, fragmentModule);

    PipelineVertexInputState vertexInputState =
        createPipelineVertexInputState(descriptor.vertexInputs);

    VkPipelineInputAssemblyStateCreateInfo inputAssembly      = createPipelineInputAssemblyState();

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = createPipelineViewportState();

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo =
        createPipelineRasterizationState(descriptor.frontFace);

    VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo =
        createPipelineMultisampleState(descriptor.msaaSamples);

    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo =
        createPipelineDepthStencilState();

    VkPipelineColorBlendAttachmentState colorState =
        createPipelineColorBlendAttachmentState(descriptor.blendEnable);

    VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo =
        createPipelineColorBlendState(&colorState);

    PipelineDynamicState dynamicState               = createPipelineDynamicState();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext               = &pipelineRendering.createInfo,
        .flags               = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT,
        .stageCount          = shaderStages.size(),
        .pStages             = shaderStages.data(),
        .pVertexInputState   = &vertexInputState.createInfo,
        .pInputAssemblyState = &inputAssembly,
        .pTessellationState  = nullptr,
        .pViewportState      = &viewportStateCreateInfo,
        .pRasterizationState = &rasterizationCreateInfo,
        .pMultisampleState   = &multisamplingCreateInfo,
        .pDepthStencilState  = &depthStencilCreateInfo,
        .pColorBlendState    = &colorBlendingCreateInfo,
        .pDynamicState       = &dynamicState.createInfo,
        .layout              = layout,
        .renderPass          = nullptr,
        .subpass             = 0,
        .basePipelineHandle  = VK_NULL_HANDLE,
        .basePipelineIndex   = -1,
    };
    VkPipeline pipeline;
    checkVkResult(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                            &pipeline));
    return pipeline;
}

Pipeline createPipeline(const VkDevice& device, const PipelineLayoutDescriptor&& layoutDescriptor,
                        const PipelineDescriptor&& pipelineDescriptor,
                        const VkShaderModule&& vertexModule, const VkShaderModule&& fragmentModule)
{
    const auto layout = createPipelineLayout(device, std::move(layoutDescriptor));
    return {
        .layout         = layout,
        .vkPipeline     = createDynamicRenderingPipeline(device, std::move(pipelineDescriptor),
                                                         layout.vkLayout, vertexModule, fragmentModule),
        .vertexModule   = vertexModule,
        .fragmentModule = fragmentModule,
    };
}

void destroyPipeline(const VkDevice& device, const Pipeline& pipeline)
{
    vkDestroyPipeline(device, pipeline.vkPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipeline.layout.vkLayout, nullptr);
    for (const auto& setLayout: pipeline.layout.setLayouts)
    {
        vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
    }
    vkDestroyShaderModule(device, pipeline.vertexModule, nullptr);
    vkDestroyShaderModule(device, pipeline.fragmentModule, nullptr);
}

} // namespace figment::vulkan
