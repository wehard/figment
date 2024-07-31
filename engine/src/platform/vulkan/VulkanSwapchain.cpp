#include "VulkanSwapchain.h"

namespace Figment
{
    static VkImageView CreateVkImageView(VkDevice device, VkImage image, VkFormat format,
            VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = format;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        VkResult result = vkCreateImageView(device, &viewCreateInfo, nullptr, &imageView);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create an Image View!");
        return imageView;
    }

    VulkanSwapchain::VulkanSwapchain(VkDevice device, const VulkanSwapchainDescriptor &&descriptor)
    {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = descriptor.Surface;
        swapchainCreateInfo.imageFormat = descriptor.SurfaceFormat;
        swapchainCreateInfo.imageColorSpace = descriptor.SurfaceColorSpace;
        swapchainCreateInfo.presentMode = descriptor.PresentMode;
        swapchainCreateInfo.imageExtent = descriptor.Extent;
        swapchainCreateInfo.minImageCount = descriptor.ImageCount;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.preTransform = descriptor.Transform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.clipped = VK_TRUE;

        CheckVkResult(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &m_Swapchain));

        CheckVkResult(vkGetSwapchainImagesKHR(device, m_Swapchain, &m_ImageCount, nullptr));
        m_Images.resize(m_ImageCount);
        CheckVkResult(vkGetSwapchainImagesKHR(device, m_Swapchain, &m_ImageCount, m_Images.data()));

        for (size_t i = 0; i < m_ImageCount; i++)
        {
            m_ImageViews[i] = CreateVkImageView(device, m_Images[i], descriptor.SurfaceFormat,
                    VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    VulkanSwapchain::~VulkanSwapchain()
    {

    }
}
