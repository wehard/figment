#include "Renderer.h"

namespace Figment::Vulkan
{
    Renderer::Renderer(const VulkanContext &context) : m_Context(context)
    {
        m_Shader = std::make_unique<VulkanShader>(context, "res/shader.vert.spv", "res/shader.frag.spv");

        m_OpaquePass = std::make_unique<VulkanRenderPass>(context, VulkanRenderPass::RenderPassDescriptor {
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

        VkExtent2D m_SwapchainExtent = m_Context.SurfaceDetails().surfaceCapabilities.currentExtent;

        m_OpaquePipeline = std::make_unique<VulkanPipeline>(context, VulkanPipeline::PipelineDescriptor {
                .ViewportWidth = m_SwapchainExtent.width,
                .ViewportHeight = m_SwapchainExtent.height,
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

        renderPassBeginInfo.framebuffer = m_Context.GetCurrentFramebuffer();

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
}
