#pragma once

#include "VulkanContext.h"

namespace Figment
{
    struct VulkanSwapchainDescriptor
    {
        VkSurfaceKHR Surface;
        VkFormat SurfaceFormat;
        VkColorSpaceKHR SurfaceColorSpace;
        VkPresentModeKHR PresentMode;
        VkExtent2D Extent;
        uint32_t ImageCount;
        VkSurfaceTransformFlagBitsKHR Transform;
    };

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain() = delete;
        explicit VulkanSwapchain(VkDevice device, const VulkanSwapchainDescriptor &&descriptor);
        ~VulkanSwapchain();

        [[nodiscard]] VkSwapchainKHR GetSwapchain() const { return m_Swapchain; }
        [[nodiscard]] VkExtent2D GetExtent() const { return m_Extent; }
        [[nodiscard]] VkFormat GetFormat() const { return m_Format; }
        [[nodiscard]] std::vector<VkImage> GetImages() const { return m_Images; }
        [[nodiscard]] std::vector<VkImageView> GetImageViews() const { return m_ImageViews; }
    private:
        VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkExtent2D m_Extent = { 0, 0 };
        VkFormat m_Format = VK_FORMAT_UNDEFINED;
        std::vector<VkImage> m_Images;
        std::vector<VkImageView> m_ImageViews;
        uint32_t m_ImageCount = 0;
    };
}
