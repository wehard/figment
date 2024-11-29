#pragma once

#include "BaseWindow.h"
#include "swapchain.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
class Window: public figment::Window
{
public:
    Window(const std::string& title, uint32_t width, uint32_t height);

    VkSurfaceCapabilitiesKHR surfaceCapabilities() const { return surfaceCapabilitiesKhr; }
    VkImageView swapChainImageView() const { return _swapchain.imageViews[imageIndex]; }
    VkImage& swapchainImage() { return _swapchain.images[imageIndex]; }
    Swapchain swapchain() const { return _swapchain; }

    VkSurfaceFormatKHR surfaceFormat = {};

private:
    Swapchain _swapchain                            = {};
    VkSurfaceKHR surface                            = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR surfaceCapabilitiesKhr = {};
    uint32_t imageIndex                             = 0;
};
} // namespace figment::vulkan
