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
    uint32_t m_ImageCount = 0;
};

Swapchain createSwapchain(const VkDevice& device, const SwapchainDescriptor&& descriptor);
uint32_t swapchainNextImageIndex(const VkDevice& device, const VkSwapchainKHR& swapchain,
                                 const VkSemaphore& presentSemaphore);
} // namespace figment::vulkan

/*
class Swapchain
{
public:
    struct Descriptor
    {
        VkSurfaceKHR surface;
        VkFormat surfaceFormat;
        VkColorSpaceKHR surfaceColorSpace;
        VkPresentModeKHR presentMode;
        VkExtent2D extent;
        uint32_t imageCount;
        VkSurfaceTransformFlagBitsKHR transform;
    };
    Swapchain() = delete;
    explicit Swapchain(VkDevice device, const Descriptor&& descriptor);
    ~Swapchain();

    uint32_t getNextImageIndex(VkSemaphore presentSemaphore);

    [[nodiscard]] VkSwapchainKHR vkSwapchain() const { return swapchain; }
    [[nodiscard]] VkExtent2D vkExtent() const { return extent; }
    [[nodiscard]] VkFormat vkFormat() const { return format; }
    [[nodiscard]] std::vector<VkImage> vkImages() const { return images; }
    [[nodiscard]] uint32_t imageCount() const { return m_ImageCount; }
    [[nodiscard]] std::vector<VkImageView> vkImageViews() const { return imageViews; }

private:
    VkDevice device;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent        = {0, 0};
    VkFormat format          = VK_FORMAT_UNDEFINED;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    uint32_t m_ImageCount = 0;
};
*/
