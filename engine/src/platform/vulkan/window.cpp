#include "window.h"
#include "context.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include "utils.h"

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
    const auto vkCtx = GetContext<vulkan::Context>();

    // Create window surface
    if (glfwCreateWindowSurface(vkCtx->GetInstance(), window, nullptr, &surface) != VK_SUCCESS)
    {
        spdlog::error("Failed to create window surface");
    }

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkCtx->GetPhysicalDevice(), surface,
                                              &surfaceCapabilitiesKhr);

    const auto details = getSurfaceDetails(vkCtx->GetPhysicalDevice(), surface);

    const auto formats = details.formats[0];

    {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface                  = surface;
        swapchainCreateInfo.imageFormat              = formats.format;
        swapchainCreateInfo.imageColorSpace          = formats.colorSpace;
        swapchainCreateInfo.presentMode              = VK_PRESENT_MODE_FIFO_KHR;
        swapchainCreateInfo.imageExtent              = {width, height};
        swapchainCreateInfo.minImageCount            = 2;
        swapchainCreateInfo.imageArrayLayers         = 1;
        swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform             = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped                  = VK_TRUE;

        checkVkResult(
            vkCreateSwapchainKHR(vkCtx->GetDevice(), &swapchainCreateInfo, nullptr, &swapchain));

        checkVkResult(vkGetSwapchainImagesKHR(vkCtx->GetDevice(), swapchain, &imageCount, nullptr));
        images.resize(imageCount);
        checkVkResult(
            vkGetSwapchainImagesKHR(vkCtx->GetDevice(), swapchain, &imageCount, images.data()));

        imageViews.resize(imageCount);
        for (size_t i = 0; i < imageCount; i++)
        {
            imageViews[i] = createVkImageView(vkCtx->GetDevice(), images[i], formats.format,
                                              VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    spdlog::info("Vulkan window created");
}

void Window::nextImage()
{
    const auto vkCtx = GetContext<vulkan::Context>();
    checkVkResult(vkAcquireNextImageKHR(vkCtx->GetDevice(), swapchain,
                                        std::numeric_limits<uint64_t>::max(), VK_NULL_HANDLE,
                                        VK_NULL_HANDLE, &imageIndex));
}

void Window::present()
{
    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 0;
    presentInfo.pWaitSemaphores    = nullptr;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &imageIndex;

    const auto vkCtx               = GetContext<vulkan::Context>();

    checkVkResult(vkQueuePresentKHR(vkCtx->GetGraphicsQueue(), &presentInfo));
}
} // namespace figment::vulkan
