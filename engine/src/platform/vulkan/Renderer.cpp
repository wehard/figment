#include "Renderer.h"
#include "VulkanSwapchain.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace Figment::Vulkan
{
    Renderer::Renderer(const VulkanContext &context) : m_Context(context),
            m_ResourceManager(context, { .Buffers = 128 })
    {
        m_Shader = std::make_unique<VulkanShader>(context, "res/shader.vert.spv", "res/shader.frag.spv");

        CreateRenderPass();
        CreatePipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffers();

        CreateGuiRenderPass();
        CreateGuiPipeline();
        CreateGuiFramebuffers();
        CreateGuiCommandPool();
        CreateGuiCommandBuffers();

        CreateDescriptorPool();
        CreateGlobalUniformBuffers();
        CreateDescriptorSets();

        CreateSynchronizationObjects();
    }

    void Renderer::Init()
    {
    }

    void Renderer::InitGui(GLFWwindow *window)
    {
        m_GuiContext = ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_InitInfo initInfo = {};
        initInfo.UseDynamicRendering = false;
        initInfo.Device = m_Context.GetDevice();
        initInfo.Instance = m_Context.GetInstance();
        initInfo.PhysicalDevice = m_Context.GetPhysicalDevice();
        initInfo.Queue = m_Context.GetGraphicsQueue();
        initInfo.QueueFamily = 0;
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPool = m_Context.CreateDescriptorPool({
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        }, 1000);
        initInfo.RenderPass = GetGuiRenderPass();
        initInfo.Subpass = 0;
        initInfo.MinImageCount = m_Context.SurfaceDetails().surfaceCapabilities.minImageCount;
        initInfo.ImageCount = m_Context.SurfaceDetails().surfaceCapabilities.minImageCount + 1;
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = CheckVkResult;
        ImGui_ImplVulkan_Init(&initInfo);

        auto commandBuffer = m_Context.BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture();
        m_Context.EndSingleTimeCommands(commandBuffer);
    }

    void Renderer::Shutdown()
    {
        std::vector<VkFence> fences;
        fences.reserve(m_SynchronizationObjects.size());
        for (auto &synchronizationObject : m_SynchronizationObjects)
        {
            fences.push_back(synchronizationObject.FenceDraw);
        }
        vkWaitForFences(m_Context.GetDevice(), fences.size(), fences.data(), VK_TRUE,
                std::numeric_limits<uint64_t>::max());
        m_DeletionQueue.Flush();

        for (auto &framebuffer : m_Framebuffers)
        {
            vkDestroyFramebuffer(m_Context.GetDevice(), framebuffer, nullptr);
        }

        for (auto &framebuffer : m_GuiFramebuffers)
        {
            vkDestroyFramebuffer(m_Context.GetDevice(), framebuffer, nullptr);
        }

        Log::Info("Renderer shutdown");

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_GuiContext);
    }

    void Renderer::Begin(const Camera &camera)
    {
        VkCommandBufferBeginInfo bufferBeginInfo = {};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_OpaquePass->Get();
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_Context.GetSwapchainExtent();
        VkClearValue clearValues[] = {
                { 0.1f, 0.1f, 0.1f, 1.0f }};
        renderPassBeginInfo.pClearValues = clearValues;
        renderPassBeginInfo.clearValueCount = 1;

        renderPassBeginInfo.framebuffer = m_Framebuffers[m_ImageIndex];

        VkCommandBuffer commandBuffer = m_CommandBuffers[m_FrameIndex];
        CheckVkResult(vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo));

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo,
                VK_SUBPASS_CONTENTS_INLINE);
    }

    void Renderer::Draw(VulkanBuffer &buffer, glm::mat4 transform, Camera &camera)
    {
        VkCommandBuffer commandBuffer = m_CommandBuffers[m_FrameIndex];
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_OpaquePipeline->Get());

        GlobalUniformData ubo = {
                .Model = transform,
                .View = camera.GetViewMatrix(),
                .Projection = camera.GetProjectionMatrix(),
        };
        // m_UBO.Projection[1][1] *= -1;
        m_GlobalUniformBuffers[m_FrameIndex]->SetData(&ubo, sizeof(GlobalUniformData));

        VkBuffer buffers[] = { buffer.Get() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_OpaquePipeline->GetLayout(),
                0, 1, m_BindGroups[m_FrameIndex]->Get(), 0, nullptr);
        vkCmdDraw(commandBuffer, 6, 1, 0, 0);
    }

    void Renderer::End()
    {
        VkCommandBuffer commandBuffer = m_CommandBuffers[m_FrameIndex];
        vkCmdEndRenderPass(commandBuffer);

        CheckVkResult(vkEndCommandBuffer(commandBuffer));
    }

    void Renderer::OnResize(uint32_t width, uint32_t height)
    {
        CreateFramebuffers();
        CreatePipeline();

        CreateGuiFramebuffers();
        CreateGuiPipeline();
    }

    void Renderer::CreateRenderPass()
    {
        m_OpaquePass = std::make_unique<VulkanRenderPass>(m_Context, VulkanRenderPass::RenderPassDescriptor {
                .ColorAttachment = {
                        .Format = m_Context.GetSwapchain()->GetFormat(),
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
        m_OpaquePipeline = std::make_unique<VulkanPipeline>(m_Context, VulkanPipeline::PipelineDescriptor {
                .ViewportWidth = m_Context.GetSwapchainExtent().width,
                .ViewportHeight = m_Context.GetSwapchainExtent().height,
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

    void Renderer::CreateFramebuffers()
    {
        m_Framebuffers.resize(m_Context.GetSwapchainImageCount());
        for (size_t i = 0; i < m_Framebuffers.size(); i++)
        {
            std::array<VkImageView, 1> attachments = { m_Context.GetSwapchainImageViews()[i] };

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = m_OpaquePass->Get();
            framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferCreateInfo.pAttachments = attachments.data();
            framebufferCreateInfo.width = m_Context.GetSwapchainExtent().width;
            framebufferCreateInfo.height = m_Context.GetSwapchainExtent().height;
            framebufferCreateInfo.layers = 1;

            CheckVkResult(vkCreateFramebuffer(m_Context.GetDevice(), &framebufferCreateInfo, nullptr,
                    &m_Framebuffers[i]));
        }
    }

    void Renderer::CreateGuiRenderPass()
    {
        m_GuiPass = std::make_unique<VulkanRenderPass>(m_Context, VulkanRenderPass::RenderPassDescriptor {
                .ColorAttachment = {
                        .Format= VK_FORMAT_B8G8R8A8_UNORM,
                        .Samples = VK_SAMPLE_COUNT_1_BIT,
                        .LoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                        .StoreOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                        .StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                        .InitialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        .FinalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                }
        });
    }

    void Renderer::CreateGuiPipeline()
    {

    }

    void Renderer::CreateGuiFramebuffers()
    {
        m_GuiFramebuffers.resize(m_Context.GetSwapchainImageCount());
        for (size_t i = 0; i < m_GuiFramebuffers.size(); i++)
        {
            std::array<VkImageView, 1> attachments = { m_Context.GetSwapchainImageViews()[i] };

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = m_GuiPass->Get();
            framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferCreateInfo.pAttachments = attachments.data();
            framebufferCreateInfo.width = m_Context.GetSwapchainExtent().width;
            framebufferCreateInfo.height = m_Context.GetSwapchainExtent().height;
            framebufferCreateInfo.layers = 1;

            CheckVkResult(vkCreateFramebuffer(m_Context.GetDevice(), &framebufferCreateInfo, nullptr,
                    &m_GuiFramebuffers[i]));
        }
    }

    void Renderer::CreateGlobalUniformBuffers()
    {
        GlobalUniformData uniformData = {
                .Model = glm::mat4(1.0f),
                .View = glm::mat4(1.0f),
                .Projection = glm::mat4(1.0f)
        };

        m_GlobalUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_GlobalUniformBuffers[i] = new VulkanBuffer(m_Context, {
                    .Name = "UniformBuffer",
                    .Data = &uniformData,
                    .ByteSize = sizeof(GlobalUniformData),
                    .Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    .MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            });
        }
    }

    void Renderer::CreateDescriptorSets()
    {
        m_BindGroups.resize(MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_BindGroups[i] = new VulkanBindGroup(m_Context, VulkanBindGroup::BindGroupDescriptor {
                    .DescriptorPool = m_DescriptorPool,
                    .Bindings = {
                            VulkanBindGroup::BindingDescriptor {
                                    .DescriptorSetLayoutBinding = {
                                            .binding = 0,
                                            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                            .descriptorCount = 1,
                                            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                            .pImmutableSamplers = nullptr
                                    },
                                    .Buffer = m_GlobalUniformBuffers[i],
                            }
                    }
            });
        }
    }
    void Renderer::CreateDescriptorPool()
    {
        m_DescriptorPool = m_Context.CreateDescriptorPool({
                {
                        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                        .descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
                }
        }, MAX_FRAMES_IN_FLIGHT);
    }
    void Renderer::BeginFrame()
    {
        CheckVkResult(
                vkWaitForFences(m_Context.GetDevice(), 1, &m_SynchronizationObjects[m_FrameIndex].FenceDraw, VK_TRUE,
                        std::numeric_limits<uint64_t>::max()));

        CheckVkResult(vkResetFences(m_Context.GetDevice(), 1, &m_SynchronizationObjects[m_FrameIndex].FenceDraw));

        m_ImageIndex = m_Context.GetSwapchain()->GetNextImageIndex(
                m_SynchronizationObjects[m_FrameIndex].SemaphoreImageAvailable);
    }

    void Renderer::EndFrame()
    {
        std::array<VkCommandBuffer, 2> submitCommandBuffers =
                { m_CommandBuffers[m_FrameIndex], m_GuiCommandBuffers[m_FrameIndex] };

        // submit command buffer to queue for execution
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_SynchronizationObjects[m_FrameIndex].SemaphoreImageAvailable;
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 2;
        submitInfo.pCommandBuffers = submitCommandBuffers.data();
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_SynchronizationObjects[m_FrameIndex].SemaphoreRenderFinished;

        CheckVkResult(
                vkQueueSubmit(m_Context.GetGraphicsQueue(), 1, &submitInfo,
                        m_SynchronizationObjects[m_FrameIndex].FenceDraw));

        // present image to screen when finished rendering
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_SynchronizationObjects[m_FrameIndex].SemaphoreRenderFinished;
        presentInfo.swapchainCount = 1;
        auto swapChain = m_Context.GetSwapchain()->Get();
        presentInfo.pSwapchains = &swapChain;
        presentInfo.pImageIndices = &m_ImageIndex;

        CheckVkResult(vkQueuePresentKHR(m_Context.GetGraphicsQueue(), &presentInfo));

        m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::CreateSynchronizationObjects()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        m_SynchronizationObjects.resize(MAX_FRAMES_IN_FLIGHT);

        for (auto &m_SynchronizationObject : m_SynchronizationObjects)
        {
            CheckVkResult(vkCreateSemaphore(m_Context.GetDevice(), &semaphoreCreateInfo, nullptr,
                    &m_SynchronizationObject.SemaphoreImageAvailable));
            CheckVkResult(vkCreateSemaphore(m_Context.GetDevice(), &semaphoreCreateInfo, nullptr,
                    &m_SynchronizationObject.SemaphoreRenderFinished));
            CheckVkResult(vkCreateFence(m_Context.GetDevice(), &fenceCreateInfo, nullptr,
                    &m_SynchronizationObject.FenceDraw));
        }

        m_DeletionQueue.Push([this]()
        {
            for (auto &m_SynchronizationObject : m_SynchronizationObjects)
            {
                vkDestroyFence(m_Context.GetDevice(), m_SynchronizationObject.FenceDraw, nullptr);
                vkDestroySemaphore(m_Context.GetDevice(), m_SynchronizationObject.SemaphoreImageAvailable, nullptr);
                vkDestroySemaphore(m_Context.GetDevice(), m_SynchronizationObject.SemaphoreRenderFinished, nullptr);
            }
        });
    }

    void Renderer::CreateCommandPool()
    {
        m_CommandPool = m_Context.CreateCommandPool();
    }

    void Renderer::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = m_CommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can only be executed by queue, not like secondary that can be executed by primary command buffers
        commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

        CheckVkResult(vkAllocateCommandBuffers(m_Context.GetDevice(), &commandBufferAllocateInfo,
                &m_CommandBuffers[0]));
    }

    void Renderer::CreateGuiCommandPool()
    {
        m_GuiCommandPool = m_Context.CreateCommandPool();
    }

    void Renderer::CreateGuiCommandBuffers()
    {
        m_GuiCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = m_GuiCommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can only be executed by queue, not like secondary that can be executed by primary command buffers
        commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_GuiCommandBuffers.size());

        CheckVkResult(vkAllocateCommandBuffers(m_Context.GetDevice(), &commandBufferAllocateInfo,
                &m_GuiCommandBuffers[0]));
    }

    VkCommandBuffer Renderer::GetGuiCommandBuffer() const
    {
        return m_GuiCommandBuffers[m_FrameIndex];
    }

    void Renderer::BeginGuiPass()
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();

        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            CheckVkResult((vkBeginCommandBuffer(GetGuiCommandBuffer(), &info)));
        }

        {
            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = GetGuiRenderPass();
            renderPassBeginInfo.framebuffer = GetCurrentGuiFramebuffer();
            renderPassBeginInfo.renderArea.extent.width = m_Context.GetSwapchainExtent().width;
            renderPassBeginInfo.renderArea.extent.height = m_Context.GetSwapchainExtent().height;
            VkClearValue clearValues[] = {
                    { 0.1f, 0.1f, 0.1f, 1.0f }};
            renderPassBeginInfo.pClearValues = clearValues;
            renderPassBeginInfo.clearValueCount = 1;
            vkCmdBeginRenderPass(GetGuiCommandBuffer(), &renderPassBeginInfo,
                    VK_SUBPASS_CONTENTS_INLINE);
        }
    }
    void Renderer::EndGuiPass()
    {
        ImGui::Render();
        ImGui::EndFrame();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), GetGuiCommandBuffer());

        {
            vkCmdEndRenderPass(GetGuiCommandBuffer());
            CheckVkResult(vkEndCommandBuffer(GetGuiCommandBuffer()));
        }
    }
}
