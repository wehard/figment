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
    _swapchain         = createSwapchain(renderContext->GetDevice(),
                                         {
                                             .surface           = surface,
                                             .surfaceFormat     = formats.format,
                                             .surfaceColorSpace = formats.colorSpace,
                                             .presentMode       = VK_PRESENT_MODE_IMMEDIATE_KHR,
                                             .extent            = {width, height},
                                             .imageCount        = 2,
                                             .transform         = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                                 });

    spdlog::info("Vulkan window created");
}
} // namespace figment::vulkan
