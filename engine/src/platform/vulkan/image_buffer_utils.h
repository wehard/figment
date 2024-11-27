#pragma once

#include "buffer.h"

#include <cstring>
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
inline void copyImageToBuffer(const VkCommandBuffer& commandBuffer, const VkImage& image,
                              const VkBuffer& buffer, const VkExtent2D& extent)
{
    VkBufferImageCopy copyImageToBufferInfo               = {};
    copyImageToBufferInfo.bufferOffset                    = 0;
    copyImageToBufferInfo.bufferRowLength                 = 0;
    copyImageToBufferInfo.bufferImageHeight               = 0;
    copyImageToBufferInfo.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    copyImageToBufferInfo.imageSubresource.mipLevel       = 0;
    copyImageToBufferInfo.imageSubresource.baseArrayLayer = 0;
    copyImageToBufferInfo.imageSubresource.layerCount     = 1;
    copyImageToBufferInfo.imageOffset                     = {0, 0, 0};
    copyImageToBufferInfo.imageExtent                     = {extent.width, extent.height, 1};

    vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1,
                           &copyImageToBufferInfo);
}

inline void copyBufferToBuffer(const VkCommandBuffer& commandBuffer, const VkBuffer& srcBuffer,
                               const VkBuffer& dstBuffer, const VkDeviceSize size)
{
    VkBufferCopy bufferCopy = {};
    bufferCopy.size         = size;
    bufferCopy.srcOffset    = 0;
    bufferCopy.dstOffset    = 0;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);
}

inline void copyBufferToImage(const VkCommandBuffer& commandBuffer, const VkBuffer& buffer,
                              const VkImage& image, const VkExtent3D& extent)
{
    VkBufferImageCopy bufferImageCopy               = {};
    bufferImageCopy.bufferOffset                    = 0;
    bufferImageCopy.bufferRowLength                 = 0;
    bufferImageCopy.bufferImageHeight               = 0;
    bufferImageCopy.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.mipLevel       = 0;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount     = 1;

    bufferImageCopy.imageOffset                     = {0, 0, 0};
    bufferImageCopy.imageExtent                     = extent;

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &bufferImageCopy);
}

inline void copyBufferToHostMemory(const Buffer& src, void* dst)
{
    if (src.hostPtr == nullptr)
        return;
    memcpy(dst, src.hostPtr, src.byteSize);
}
} // namespace figment::vulkan
