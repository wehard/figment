#include "window.h"
#include "context.h"
#define GLFW_INCLUDE_VULKAN
#include "utils.h"

#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <synchronization.h>

namespace figment::vulkan
{
static void glfwFramebufferSizeCallback(GLFWwindow* window, const int width, const int height)
{
    const auto figmentWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    figmentWindow->OnResize({
        .Width             = width,
        .Height            = height,
        .FramebufferWidth  = width,
        .FramebufferHeight = height,
    });
}

static VkImageView createVkImageView(const VkDevice& device, const VkImage& image,
                                     const VkFormat format, const VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewCreateInfo           = {};
    viewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image                           = image;
    viewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format                          = format;
    viewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

    viewCreateInfo.subresourceRange.aspectMask     = aspectFlags;
    viewCreateInfo.subresourceRange.baseMipLevel   = 0;
    viewCreateInfo.subresourceRange.levelCount     = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount     = 1;

    VkImageView imageView;
    checkVkResult(vkCreateImageView(device, &viewCreateInfo, nullptr, &imageView));
    return imageView;
}

static Context::VulkanSurfaceDetails getSurfaceDetails(const VkPhysicalDevice& device,
                                                       const VkSurfaceKHR& surface)
{
    Context::VulkanSurfaceDetails surfaceDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceDetails.surfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        surfaceDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                             surfaceDetails.formats.data());
    }

    uint32_t presentationCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);
    if (presentationCount != 0)
    {
        surfaceDetails.presentationModes.resize(presentationCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount,
                                                  surfaceDetails.presentationModes.data());
    }
    return surfaceDetails;
}

Window::Window(const std::string& title, const uint32_t width, const uint32_t height):
    figment::Window(title, width, height)
{
    if (!glfwInit())
    {
        spdlog::error("Failed to initialize GLFW");
    }

    if (glfwVulkanSupported())
    {
        spdlog::info("Vulkan is supported");
    }
    else
    {
        spdlog::error("Vulkan is not supported");
    }

    spdlog::info("GLFW version: {}", glfwGetVersionString());

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(),
                              nullptr, nullptr);

    if (!window)
    {
        spdlog::error("Failed to create GLFW window");
    }

    glfwShowWindow(window);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
    glfwSetWindowUserPointer(window, this);

    renderContext = std::make_shared<vulkan::Context>(window);
    renderContext->Init(m_Width, m_Height);
    const auto renderContext = GetContext<vulkan::Context>();

    // Create window surface
    if (glfwCreateWindowSurface(renderContext->GetInstance(), window, nullptr, &surface) !=
        VK_SUCCESS)
    {
        spdlog::error("Failed to create window surface");
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(renderContext->GetPhysicalDevice(), surface,
                                              &surfaceCapabilitiesKhr);

    const auto details = getSurfaceDetails(renderContext->GetPhysicalDevice(), surface);

    const auto formats = details.formats[0];
    surfaceFormat      = formats;

    {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface                  = surface;
        swapchainCreateInfo.imageFormat              = formats.format;
        swapchainCreateInfo.imageColorSpace          = formats.colorSpace;
        swapchainCreateInfo.presentMode              = VK_PRESENT_MODE_IMMEDIATE_KHR;
        swapchainCreateInfo.imageExtent              = {width, height};
        swapchainCreateInfo.minImageCount            = 2;
        swapchainCreateInfo.imageArrayLayers         = 1;
        swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform             = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped                  = VK_TRUE;

        checkVkResult(vkCreateSwapchainKHR(renderContext->GetDevice(), &swapchainCreateInfo,
                                           nullptr, &swapchain));

        checkVkResult(
            vkGetSwapchainImagesKHR(renderContext->GetDevice(), swapchain, &imageCount, nullptr));
        images.resize(imageCount);
        checkVkResult(vkGetSwapchainImagesKHR(renderContext->GetDevice(), swapchain, &imageCount,
                                              images.data()));

        imageViews.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++)
        {
            imageViews[i] = createVkImageView(renderContext->GetDevice(), images[i], formats.format,
                                              VK_IMAGE_ASPECT_COLOR_BIT);
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            renderFinishedSemaphores.emplace_back(createSemaphore(
                renderContext->GetDevice(), fmt::format("renderFinishedSemaphore {}", i).c_str()));
            fences.emplace_back(
                createFence(renderContext->GetDevice(), true, fmt::format("fence {}", i).c_str()));
            imageAvailableSemaphores.emplace_back(createSemaphore(
                renderContext->GetDevice(), fmt::format("imageAvailableSemaphore {}", i).c_str()));
        }
        const auto commandPool = renderContext->createCommandPool();
        commandBuffers.resize(Window::MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < Window::MAX_FRAMES_IN_FLIGHT; i++)
        {
            VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
            commandBufferAllocateInfo.sType       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool = commandPool;
            commandBufferAllocateInfo.level       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = 1;

            checkVkResult(vkAllocateCommandBuffers(renderContext->GetDevice(),
                                                   &commandBufferAllocateInfo, &commandBuffers[i]));
        }
    }

    spdlog::info("Vulkan window created");
}

void Window::nextImage()
{
    const auto vkCtx = GetContext<vulkan::Context>();
    debug::beginLabel(vkCtx->GetGraphicsQueue(), "Frame");

    vkWaitForFences(vkCtx->GetDevice(), 1, &fences[frameIndex], VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    vkResetFences(vkCtx->GetDevice(), 1, &fences[frameIndex]);
    checkVkResult(
        vkAcquireNextImageKHR(vkCtx->GetDevice(), swapchain, std::numeric_limits<uint64_t>::max(),
                              imageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &imageIndex));
}

void Window::render(const VkCommandBuffer& commandBuffer)
{
    const auto vkCtx                  = GetContext<vulkan::Context>();

    VkSubmitInfo submitInfo           = {};
    submitInfo.sType                  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[]      = {imageAvailableSemaphores[frameIndex]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount     = 1;
    submitInfo.pWaitSemaphores        = waitSemaphores;
    submitInfo.pWaitDstStageMask      = waitStages;
    submitInfo.commandBufferCount     = 1;
    submitInfo.pCommandBuffers        = &commandBuffer;
    VkSemaphore signalSemaphores[]    = {renderFinishedSemaphores[frameIndex]};
    submitInfo.signalSemaphoreCount   = 1;
    submitInfo.pSignalSemaphores      = signalSemaphores;

    debug::endLabel(vkCtx->GetGraphicsQueue());
    checkVkResult(vkQueueSubmit(vkCtx->GetGraphicsQueue(), 1, &submitInfo, fences[frameIndex]));

    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &renderFinishedSemaphores[frameIndex];
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &imageIndex;

    checkVkResult(vkQueuePresentKHR(vkCtx->GetGraphicsQueue(), &presentInfo));
    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
} // namespace figment::vulkan
