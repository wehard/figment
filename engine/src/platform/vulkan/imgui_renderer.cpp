#include "imgui_renderer.h"
#include "synchronization.h"
#include "utils.h"

#include <image_transition.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace figment::vulkan
{
ImGuiRenderer::ImGuiRenderer(const Descriptor&& descriptor)
{
    vkCmdBeginRenderingKHR = reinterpret_cast<PFN_vkCmdBeginRenderingKHR>(
        vkGetInstanceProcAddr(descriptor.instance, "vkCmdBeginRenderingKHR"));
    vkCmdEndRenderingKHR = reinterpret_cast<PFN_vkCmdEndRenderingKHR>(
        vkGetInstanceProcAddr(descriptor.instance, "vkCmdEndRenderingKHR"));

    imguiContext = ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(descriptor.window, true);
    ImGui_ImplVulkan_InitInfo initInfo   = {};
    initInfo.UseDynamicRendering         = true;
    initInfo.PipelineRenderingCreateInfo = {
        .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
        .pNext                   = nullptr,
        .viewMask                = 0,
        .colorAttachmentCount    = 1,
        .pColorAttachmentFormats = &descriptor.swapchainFormat,
        .depthAttachmentFormat   = VK_FORMAT_UNDEFINED,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };
    initInfo.Instance        = descriptor.instance;
    initInfo.Device          = descriptor.device;
    initInfo.PhysicalDevice  = descriptor.physicalDevice;
    initInfo.Queue           = descriptor.queue;
    initInfo.QueueFamily     = 0;
    initInfo.PipelineCache   = VK_NULL_HANDLE;
    initInfo.DescriptorPool  = descriptor.descriptorPool;
    initInfo.RenderPass      = nullptr;
    initInfo.Subpass         = 0;
    initInfo.MinImageCount   = descriptor.minImageCount;
    initInfo.ImageCount      = descriptor.minImageCount + 1;
    initInfo.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator       = nullptr;
    initInfo.CheckVkResultFn = checkVkResult;
    ImGui_ImplVulkan_Init(&initInfo);

    commandBuffers.resize(Window::MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < Window::MAX_FRAMES_IN_FLIGHT; i++)
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = descriptor.commandPool;
        commandBufferAllocateInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        checkVkResult(vkAllocateCommandBuffers(descriptor.device, &commandBufferAllocateInfo,
                                               &commandBuffers[i]));
    }

    {
        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffers[0], &commandBufferBeginInfo);
        ImGui_ImplVulkan_CreateFontsTexture();
        vkEndCommandBuffer(commandBuffers[0]);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        vkQueueSubmit(descriptor.queue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(descriptor.queue);
        vkResetCommandBuffer(commandBuffers[0], 0);
    }

    device = descriptor.device;
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);
}

void ImGuiRenderer::begin(const VkImage& renderTarget, const VkImageView& renderTargetView,
                          const VkRect2D renderArea, const uint32_t frameIndex)
{
    rt = renderTarget;

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    {
        vkResetCommandBuffer(commandBuffers[frameIndex], 0);

        VkCommandBufferBeginInfo bufferBeginInfo = {};
        bufferBeginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        vkBeginCommandBuffer(commandBuffers[frameIndex], &bufferBeginInfo);
    }

    transitionImageLayout(commandBuffers[frameIndex],
                          renderTarget,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    {
        constexpr VkClearValue colorClearValue           = {.color = {0.1f, 0.1f, 0.15f, 1.0f}};

        VkRenderingAttachmentInfoKHR colorAttachmentInfo = {};
        colorAttachmentInfo.sType          = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorAttachmentInfo.imageView      = renderTargetView;
        colorAttachmentInfo.imageLayout    = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
        colorAttachmentInfo.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachmentInfo.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachmentInfo.clearValue     = colorClearValue;

        VkRenderingInfoKHR renderingInfo   = {};
        renderingInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        renderingInfo.renderArea           = renderArea;
        renderingInfo.layerCount           = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments    = &colorAttachmentInfo;
        renderingInfo.pDepthAttachment     = nullptr;

        vkCmdBeginRenderingKHR(commandBuffers[frameIndex], &renderingInfo);
    }
}

void ImGuiRenderer::end(const uint32_t frameIndex) const
{

    ImGui::Render();
    ImGui::EndFrame();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[frameIndex]);
    vkCmdEndRenderingKHR(commandBuffers[frameIndex]);

    transitionImageLayout(commandBuffers[frameIndex],
                          rt,
                          VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                          VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    vkEndCommandBuffer(commandBuffers[frameIndex]);
}
} // namespace figment::vulkan
