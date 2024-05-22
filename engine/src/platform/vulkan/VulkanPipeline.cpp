#include "VulkanPipeline.h"

namespace Figment
{
    VulkanPipeline::VulkanPipeline(const VulkanContext &context, const PipelineDescriptor &&descriptor) : m_Context(
            context)
    {
        CreatePipeline(descriptor);
    }

    VulkanPipeline::~VulkanPipeline()
    {
        vkDestroyPipeline(m_Context.GetDevice(), m_Pipeline, nullptr);
    }

    void VulkanPipeline::CreatePipeline(const PipelineDescriptor &descriptor)
    {
        // shader stage create info for graphics pipeline
        VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
        vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexShaderCreateInfo.module = descriptor.VertexModule;
        vertexShaderCreateInfo.pName = "main"; // points to first glsl function called

        VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
        fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentShaderCreateInfo.module = descriptor.FragmentModule;
        fragmentShaderCreateInfo.pName = "main"; // points to first glsl function called

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo };

        // create pipeline

        // vertex data description (pos, col, uv, normal etc.)
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = descriptor.VertexInput.Binding;
        bindingDescription.stride = descriptor.VertexInput.Stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // modify if instanced drawing

        // -- vertex input --
        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
        vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(descriptor.VertexInput.Attributes.size());
        vertexInputCreateInfo.pVertexAttributeDescriptions = descriptor.VertexInput.Attributes.data();

        // -- input assembly --
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // -- viewport & scissor --
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)descriptor.ViewportWidth;
        viewport.height = (float)descriptor.ViewportHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = { 0, 0 };
        scissor.extent = { .width = descriptor.ViewportWidth, .height = descriptor.ViewportHeight };

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;

        // -- dynamic states -- (for dynamic resizing of viewport etc. using cmd buffers vkCmdSetViewport())
        // std::vector<VkDynamicState> dynamicStateEnables;
        // dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        // dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);

        // VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
        // dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        // dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
        // dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();

        // -- rasterizer --
        VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {};
        rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationCreateInfo.lineWidth = 1.0f;
        rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizationCreateInfo.depthBiasEnable = VK_FALSE;

        // -- multisampling -- (disabled)
        VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
        multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
        multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        // -- blending --
        // blend attachment state
        VkPipelineColorBlendAttachmentState colorState = {};
        colorState.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
                        | VK_COLOR_COMPONENT_A_BIT;
        colorState.blendEnable = VK_TRUE;

        // blend equation (srcColorBlendFactor * newcolor) colorBlendOp (dstColorBlendFactor * oldcolor)
        colorState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorState.colorBlendOp = VK_BLEND_OP_ADD; // (VK_BLEND_FACTOR_SRC_ALPHA * newcolor) + (VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA * oldcolor)
        colorState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorState.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo = {};
        colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendingCreateInfo.attachmentCount = 1;
        colorBlendingCreateInfo.pAttachments = &colorState;

        // -- pipeline layout -- (descriptor set layouts) uniforms etc.
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = nullptr;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        // create pipeline layout
        VkPipelineLayout pipelineLayout;
        VkResult result = vkCreatePipelineLayout(m_Context.GetDevice(), &pipelineLayoutCreateInfo, nullptr,
                &pipelineLayout);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create pipeline layout!");

        // -- depth stencil testing --
        // set up depth stencil testing

        // create graphics pipeline
        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = shaderStages;
        pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pDynamicState = nullptr;
        pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
        pipelineCreateInfo.pDepthStencilState = nullptr;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = descriptor.RenderPass;
        pipelineCreateInfo.subpass = 0;

        // pipeline derivatives : used to create new pipelines based on others
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        result = vkCreateGraphicsPipelines(m_Context.GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                &m_Pipeline);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create graphics pipeline!");
    }
}
