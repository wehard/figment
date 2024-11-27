#include "imgui_renderer.h"
#include "utils.h"
#include "window.h"

#include <VulkanSwapchain.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <FileUtils.h>
#include <BaseWindow.h>

namespace figment::vulkan
{
ImGuiRenderer::ImGuiRenderer(const vulkan::Window& window):
    window(window), ctx(*window.GetContext<Context>())
{
    const std::array<VkFormat, 1> colorAttachmentFormats = {VK_FORMAT_B8G8R8A8_UNORM};
    imguiContext                                         = ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window.GetNative(), true);
    ImGui_ImplVulkan_InitInfo initInfo   = {};
    initInfo.UseDynamicRendering         = true;
    initInfo.PipelineRenderingCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .pNext                   = nullptr,
        .viewMask                = 0,
        .colorAttachmentCount    = colorAttachmentFormats.size(),
        .pColorAttachmentFormats = colorAttachmentFormats.data(),
        .depthAttachmentFormat   = VK_FORMAT_D32_SFLOAT,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };
    initInfo.Device         = ctx.GetDevice();
    initInfo.Instance       = ctx.GetInstance();
    initInfo.PhysicalDevice = ctx.GetPhysicalDevice();
    initInfo.Queue          = ctx.GetGraphicsQueue();
    initInfo.QueueFamily    = 0;
    initInfo.PipelineCache  = VK_NULL_HANDLE;
    initInfo.DescriptorPool =
        ctx.createDescriptorPool({{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
                                  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
                                  {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
                                  {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
                                  {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
                                  {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
                                  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
                                  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
                                  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
                                  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
                                  {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}},
                                 1000);
    initInfo.RenderPass      = nullptr;
    initInfo.Subpass         = 0;
    initInfo.MinImageCount   = window.surfaceCapabilities().minImageCount;
    initInfo.ImageCount      = window.surfaceCapabilities().minImageCount + 1;
    initInfo.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator       = nullptr;
    initInfo.CheckVkResultFn = checkVkResult;
    ImGui_ImplVulkan_Init(&initInfo);

    auto stCommandBuffer = ctx.beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture();
    ctx.endSingleTimeCommands(stCommandBuffer);

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool        = ctx.createCommandPool();
    commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    checkVkResult(
        vkAllocateCommandBuffers(ctx.GetDevice(), &commandBufferAllocateInfo, &commandBuffer));

    depthAttachment = createTexture(ctx.GetDevice(),
                                    ctx.GetPhysicalDevice(),
                                    {
                                        .debugName  = "ImGuiDepthAttachment",
                                        .imageType  = VK_IMAGE_TYPE_2D,
                                        .width      = window.GetWidth(),
                                        .height     = window.GetHeight(),
                                        .depth      = 1,
                                        .format     = VK_FORMAT_D32_SFLOAT,
                                        .usage      = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                        .sample     = true,
                                        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                                        .mipLevels  = 1,
                                        .samples    = VK_SAMPLE_COUNT_1_BIT,
                                    });
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);
}

void ImGuiRenderer::beginFrame()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    {
        vkResetCommandBuffer(commandBuffer, 0);

        VkCommandBufferBeginInfo bufferBeginInfo = {};
        bufferBeginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffer, &bufferBeginInfo);
    }

    {
        constexpr VkClearValue colorClearValue = {.color = {0.0f, 0.0f, 0.0f, 0.0f}};

        constexpr VkClearValue depthClearValue = {
            .depthStencil = {1.0f, 0},
        };

        VkRenderingAttachmentInfoKHR colorAttachmentInfo = {};
        colorAttachmentInfo.sType       = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachmentInfo.imageView   = window.swapChainImageView();
        colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        colorAttachmentInfo.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentInfo.clearValue  = colorClearValue;

        VkRenderingAttachmentInfoKHR depthAttachmentInfo = {};
        depthAttachmentInfo.sType        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthAttachmentInfo.imageView    = depthAttachment.view;
        depthAttachmentInfo.imageLayout  = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depthAttachmentInfo.loadOp       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachmentInfo.storeOp      = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachmentInfo.clearValue   = depthClearValue;

        VkRenderingInfoKHR renderingInfo = {};
        renderingInfo.sType              = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.renderArea         = {
                    .offset = {0, 0},
                    .extent = window.surfaceCapabilities().currentExtent,
        };
        renderingInfo.layerCount           = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments    = &colorAttachmentInfo;
        renderingInfo.pDepthAttachment     = &depthAttachmentInfo;

        ctx.vkCmdBeginRenderingKHR(commandBuffer, &renderingInfo);
    }
}

void ImGuiRenderer::endFrame()
{

    ImGui::Render();
    ImGui::EndFrame();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    ctx.vkCmdEndRenderingKHR(commandBuffer);
    {
        vkEndCommandBuffer(commandBuffer);
    }

    std::array<VkCommandBuffer, 1> commandBuffers = {commandBuffer};

    VkSubmitInfo submitInfo                       = {};
    submitInfo.sType                              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount                 = 0;
    submitInfo.pWaitSemaphores                    = nullptr;
    submitInfo.pWaitDstStageMask                  = nullptr;
    submitInfo.commandBufferCount                 = 1;
    submitInfo.pCommandBuffers                    = commandBuffers.data();
    submitInfo.signalSemaphoreCount               = 0;
    submitInfo.pSignalSemaphores                  = nullptr;

    checkVkResult(vkQueueSubmit(ctx.GetGraphicsQueue(), 1, &submitInfo, nullptr));
}
} // namespace figment::vulkan
