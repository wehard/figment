#include "Renderer.h"

namespace Figment::Vulkan
{
    Renderer::Renderer(const VulkanContext &context) : m_Context(context)
    {
        m_Shader = std::make_unique<VulkanShader>(context, "res/shader.vert.spv", "res/shader.frag.spv");

        CreateRenderPass();
        CreatePipeline();
        CreateFramebuffers();
    }

    Renderer::~Renderer()
    {

    }

    void Renderer::Begin(const Camera &camera)
    {
        VkCommandBufferBeginInfo bufferBeginInfo = {};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_OpaquePass->Get();
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_Context.SurfaceDetails().surfaceCapabilities.currentExtent;
        VkClearValue clearValues[] = {
                { 0.1f, 0.1f, 0.1f, 1.0f }};
        renderPassBeginInfo.pClearValues = clearValues;
        renderPassBeginInfo.clearValueCount = 1;

        renderPassBeginInfo.framebuffer = m_Framebuffers[m_Context.GetSwapchainImageIndex()];

        VkCommandBuffer commandBuffer = m_Context.GetCurrentCommandBuffer();
        CheckVkResult(vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo));

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                VK_SUBPASS_CONTENTS_INLINE);
    }

    void Renderer::Draw(VulkanBuffer &buffer, glm::mat4 transform, Camera &camera)
    {
        VkCommandBuffer commandBuffer = m_Context.GetCurrentCommandBuffer();
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_OpaquePipeline->Get());

        VulkanContext::UniformBufferObject ubo = {
                .Model = transform,
                .View = camera.GetViewMatrix(),
                .Projection = camera.GetProjectionMatrix(),
        };
        // m_UBO.Projection[1][1] *= -1;
        m_Context.GetCurrentUniformBuffer()->SetData(&ubo, sizeof(VulkanContext::UniformBufferObject));

        VkBuffer buffers[] = { buffer.Get() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_OpaquePipeline->GetLayout(),
                0, 1, m_Context.GetCurrentBindGroup()->Get(), 0, nullptr);
        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
    }

    void Renderer::End()
    {
        VkCommandBuffer commandBuffer = m_Context.GetCurrentCommandBuffer();
        vkCmdEndRenderPass(commandBuffer);

        CheckVkResult(vkEndCommandBuffer(commandBuffer));
    }

    void Renderer::OnResize(uint32_t width, uint32_t height)
    {
        CreateFramebuffers();
        CreatePipeline();
    }

    void Renderer::CreateFramebuffers()
    {
        VkExtent2D swapchainExtent = m_Context.SurfaceDetails().surfaceCapabilities.currentExtent;

        m_Framebuffers.resize(m_Context.GetSwapchainImageCount());
        for (size_t i = 0; i < m_Framebuffers.size(); i++)
        {
            std::array<VkImageView, 1> attachments = { m_Context.GetSwapchainImageViews()[i] };

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = m_OpaquePass->Get();
            framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferCreateInfo.pAttachments = attachments.data();
            framebufferCreateInfo.width = swapchainExtent.width;
            framebufferCreateInfo.height = swapchainExtent.height;
            framebufferCreateInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(m_Context.GetDevice(), &framebufferCreateInfo, nullptr,
                    &m_Framebuffers[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    void Renderer::CreateRenderPass()
    {
        m_OpaquePass = std::make_unique<VulkanRenderPass>(m_Context, VulkanRenderPass::RenderPassDescriptor {
                .ColorAttachment = {
                        .Format = m_Context.SurfaceDetails().formats[0].format,
                        .Samples = VK_SAMPLE_COUNT_1_BIT,
                        .LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .StoreOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                        .StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                        .InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .FinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                }
        });
    }

    void Renderer::CreatePipeline()
    {
        auto swapchainExtent = m_Context.SurfaceDetails().surfaceCapabilities.currentExtent;

        m_OpaquePipeline = std::make_unique<VulkanPipeline>(m_Context, VulkanPipeline::PipelineDescriptor {
                .ViewportWidth = swapchainExtent.width,
                .ViewportHeight = swapchainExtent.height,
                .VertexInput = {
                        .Binding = 0,
                        .Stride = sizeof(Vertex),
                        .InputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                        .Attributes = {
                                {
                                        .location = 0,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                                        .offset = offsetof(Vertex, Position)
                                },
                                {
                                        .location = 1,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                                        .offset = offsetof(Vertex, Color)
                                }
                        }
                },
                .RenderPass = m_OpaquePass->Get(),
                .VertexModule = m_Shader->GetVertexModule(),
                .FragmentModule = m_Shader->GetFragmentModule(),
                .DescriptorSetLayoutBindings = {
                        {
                                .binding = 0,
                                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                .descriptorCount = 1,
                                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                .pImmutableSamplers = nullptr
                        }
                }
        });
    }
}
