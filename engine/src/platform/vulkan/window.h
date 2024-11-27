#pragma once

#include "Window.h"
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
class Window: public figment::Window
{
public:
    Window(const std::string& title, uint32_t width, uint32_t height);

    void nextImage();
    void present();
    VkSurfaceCapabilitiesKHR surfaceCapabilities() const { return surfaceCapabilitiesKhr; }
    VkImageView swapChainImageView() const { return imageViews[imageIndex]; }

private:
    VkSwapchainKHR swapchain                        = VK_NULL_HANDLE;
    VkSurfaceKHR surface                            = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR surfaceCapabilitiesKhr = {};
    uint32_t imageCount                             = 0;
    std::vector<VkImage> images                     = {};
    std::vector<VkImageView> imageViews             = {};
    uint32_t imageIndex                             = 0;
};
} // namespace figment::vulkan
