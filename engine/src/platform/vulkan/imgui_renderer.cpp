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

    device = descriptor.device;

    spdlog::info("ImGui version: {}", ImGui::GetVersion());
}

ImGuiRenderer::~ImGuiRenderer()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);
}

void ImGuiRenderer::begin(const VkCommandBuffer& commandBuffer, const RenderTarget&& target) const
{
    debug::beginLabel(commandBuffer, "ImGuiRenderer");
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    constexpr VkClearValue colorClearValue           = {.color = {0.1f, 0.1f, 0.15f, 1.0f}};

    VkRenderingAttachmentInfoKHR colorAttachmentInfo = {};
    colorAttachmentInfo.sType          = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
    colorAttachmentInfo.imageView      = target.imageView;
    colorAttachmentInfo.imageLayout    = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR;
    colorAttachmentInfo.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentInfo.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue     = colorClearValue;

    VkRenderingInfoKHR renderingInfo   = {};
    renderingInfo.sType                = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderingInfo.renderArea           = target.renderArea;
    renderingInfo.layerCount           = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments    = &colorAttachmentInfo;
    renderingInfo.pDepthAttachment     = nullptr;

    vkCmdBeginRenderingKHR(commandBuffer, &renderingInfo);

    if (showDebugInfo)
    {
        ImGui::Begin("ImGuiRenderer");
        ImGui::Text("ImGui version: %s", ImGui::GetVersion());
        ImGui::Text("Render area: %d x %d", target.renderArea.extent.width,
                    target.renderArea.extent.height);
        ImGui::Text("Vulkan device: %p", device);
        ImGui::End();
    }
}

void ImGuiRenderer::end(const VkCommandBuffer& commandBuffer) const
{
    ImGui::Render();
    ImGui::EndFrame();

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    vkCmdEndRenderingKHR(commandBuffer);

    debug::endLabel(commandBuffer);
}
} // namespace figment::vulkan
