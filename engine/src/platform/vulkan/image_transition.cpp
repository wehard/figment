#include "image_transition.h"

namespace figment::vulkan
{
struct LayoutTransitionInfo
{
    VkAccessFlags srcAccessMask;
    VkAccessFlags dstAccessMask;
    VkPipelineStageFlags srcStageMask;
    VkPipelineStageFlags dstStageMask;
    VkImageAspectFlags aspectMask;
};

LayoutTransitionInfo getLayoutTransitionInfo(const VkImageLayout oldLayout,
                                             const VkImageLayout newLayout)
{
    LayoutTransitionInfo info = {};

    switch (oldLayout)
    {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            info.srcAccessMask = VK_ACCESS_NONE;
            info.srcStageMask  = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            info.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            info.srcStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            info.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            info.srcStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            info.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            info.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            info.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            info.srcStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            info.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            info.srcStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;
        default:
            break;
    }

    switch (newLayout)
    {
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            info.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            info.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            info.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            info.dstStageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            info.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            info.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            info.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            info.dstStageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            info.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            info.dstStageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;
        default:
            break;
    }

    if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ||
        newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
    {
        info.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    else
    {
        info.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    return info;
}

void transitionImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& image,
                           const VkImageLayout oldLayout, const VkImageLayout newLayout,
                           const uint32_t baseMipLevel, const uint32_t mipLevels)
{
    const LayoutTransitionInfo transitionInfo = getLayoutTransitionInfo(oldLayout, newLayout);

    VkImageMemoryBarrier barrier              = {};
    barrier.sType                             = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout                         = oldLayout;
    barrier.newLayout                         = newLayout;
    barrier.srcQueueFamilyIndex               = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex               = VK_QUEUE_FAMILY_IGNORED;
    barrier.image                             = image;
    barrier.subresourceRange.aspectMask       = transitionInfo.aspectMask;
    barrier.subresourceRange.baseMipLevel     = baseMipLevel;
    barrier.subresourceRange.levelCount       = mipLevels;
    barrier.subresourceRange.baseArrayLayer   = 0;
    barrier.subresourceRange.layerCount       = 1;
    barrier.srcAccessMask                     = transitionInfo.srcAccessMask;
    barrier.dstAccessMask                     = transitionInfo.dstAccessMask;

    vkCmdPipelineBarrier(commandBuffer, transitionInfo.srcStageMask, transitionInfo.dstStageMask, 0,
                         0, nullptr, 0, nullptr, 1, &barrier);
}
} // namespace figment::vulkan
