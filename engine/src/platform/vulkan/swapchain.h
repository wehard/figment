#pragma once

#include "context.h"

namespace figment::vulkan
{
struct SwapchainDescriptor
{
    VkSurfaceKHR surface;
    VkFormat surfaceFormat;
    VkColorSpaceKHR surfaceColorSpace;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;
    uint32_t imageCount;
    VkSurfaceTransformFlagBitsKHR transform;
};

struct Swapchain
{
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent        = {0, 0};
    VkFormat format          = VK_FORMAT_UNDEFINED;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};

Swapchain createSwapchain(const VkDevice& device, const SwapchainDescriptor&& descriptor);
uint32_t swapchainNextImageIndex(const VkDevice& device, const VkSwapchainKHR& swapchain,
                                 const VkSemaphore& presentSemaphore);
} // namespace figment::vulkan
