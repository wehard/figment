#pragma once

#include "BaseWindow.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
class Window: public figment::Window
{
public:
    Window(const std::string& title, uint32_t width, uint32_t height);

    void nextImage();
    void render(const VkCommandBuffer& commandBuffer);

    VkSurfaceCapabilitiesKHR surfaceCapabilities() const { return surfaceCapabilitiesKhr; }
    VkImageView swapChainImageView() const { return imageViews[imageIndex]; }
    VkImage& swapchainImage() { return images[imageIndex]; }

    std::vector<VkSemaphore> renderFinishedSemaphores = {};
    std::vector<VkFence> fences                       = {};
    std::vector<VkSemaphore> imageAvailableSemaphores = {};

    constexpr static uint32_t MAX_FRAMES_IN_FLIGHT    = 2;
    uint32_t frameIndex                               = 0;
    VkSurfaceFormatKHR surfaceFormat                  = {};

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
