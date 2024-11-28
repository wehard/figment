#include "swapchain.h"
#include "utils.h"

namespace figment::vulkan
{
static VkImageView createVkImageView(VkDevice device, VkImage image, VkFormat format,
                                     VkImageAspectFlags aspectFlags)
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

Swapchain createSwapchain(const VkDevice& device, const SwapchainDescriptor&& descriptor)
{
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface                  = descriptor.surface;
    swapchainCreateInfo.imageFormat              = descriptor.surfaceFormat;
    swapchainCreateInfo.imageColorSpace          = descriptor.surfaceColorSpace;
    swapchainCreateInfo.presentMode              = descriptor.presentMode;
    swapchainCreateInfo.imageExtent              = descriptor.extent;
    swapchainCreateInfo.minImageCount            = descriptor.imageCount;
    swapchainCreateInfo.imageArrayLayers         = 1;
    swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform             = descriptor.transform;
    swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.clipped                  = VK_TRUE;

    Swapchain swapchain;
    uint32_t imageCount = 0;
    checkVkResult(
        vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain.swapchain));

    checkVkResult(vkGetSwapchainImagesKHR(device, swapchain.swapchain, &imageCount, nullptr));
    swapchain.images.resize(imageCount);
    checkVkResult(
        vkGetSwapchainImagesKHR(device, swapchain.swapchain, &imageCount, swapchain.images.data()));

    swapchain.imageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        swapchain.imageViews[i] = createVkImageView(
            device, swapchain.images[i], descriptor.surfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
    return swapchain;
}

uint32_t swapchainNextImageIndex(VkDevice const& device, VkSwapchainKHR const& swapchain,
                                 VkSemaphore const& presentSemaphore)
{
    uint32_t imageIndex = 0;
    checkVkResult(vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(),
                                        presentSemaphore, VK_NULL_HANDLE, &imageIndex));
    return imageIndex;
}
} // namespace figment::vulkan
