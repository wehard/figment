#include "texture.h"
#include "buffer.h"
#include "context.h"
#include "image_buffer_utils.h"
#include "utils.h"

#include "image_transition.h"

namespace figment::vulkan
{

static uint32_t findMemoryTypeIndex(const VkPhysicalDevice& physicalDevice,
                                    const uint32_t allowedTypes,
                                    const VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    for (auto i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((allowedTypes & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    return -1;
}

static VkImage createImage(const VkDevice& device, const VkImageType imageType,
                           const uint32_t width, const uint32_t height, const uint32_t depth,
                           const uint32_t mipLevels, const VkFormat format,
                           const VkImageUsageFlags usage, const VkSampleCountFlagBits samples)
{
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType         = imageType;
    imageCreateInfo.extent.width      = width;
    imageCreateInfo.extent.height     = height;
    imageCreateInfo.extent.depth      = depth;
    imageCreateInfo.mipLevels         = mipLevels;
    imageCreateInfo.arrayLayers       = 1;
    imageCreateInfo.format            = format;
    imageCreateInfo.tiling            = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCreateInfo.usage             = usage;
    imageCreateInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.samples           = samples;
    imageCreateInfo.flags             = 0;

    VkImage image                     = VK_NULL_HANDLE;
    checkVkResult(vkCreateImage(device, &imageCreateInfo, nullptr, &image));
    return image;
}

static VkDeviceMemory allocateMemory(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                                     const VkImage& image)
{
    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(device, image, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.allocationSize       = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex      = findMemoryTypeIndex(
        physicalDevice, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkDeviceMemory memory = VK_NULL_HANDLE;
    checkVkResult(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &memory));

    checkVkResult(vkBindImageMemory(device, image, memory, 0));
    return memory;
}

static VkImageView createImageView(const VkDevice& device, const VkImage& image,
                                   const VkImageViewType viewType, const VkFormat imageFormat,
                                   const VkImageAspectFlags aspectMask, const uint32_t mipLevels)
{
    VkImageViewCreateInfo imageViewCreateInfo           = {};
    imageViewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image                           = image;
    imageViewCreateInfo.viewType                        = viewType;
    imageViewCreateInfo.format                          = imageFormat;
    imageViewCreateInfo.subresourceRange.aspectMask     = aspectMask;
    imageViewCreateInfo.subresourceRange.baseMipLevel   = 0;
    imageViewCreateInfo.subresourceRange.levelCount     = mipLevels;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount     = 1;

    VkImageView view                                    = VK_NULL_HANDLE;
    checkVkResult(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &view));
    return view;
}

Texture createTexture(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                      const TextureDescriptor&& descriptor)
{
    validateImageFormatProperties(physicalDevice, VK_IMAGE_TILING_OPTIMAL,
                                  ImageFormatProperties{
                                      .imageType = descriptor.imageType,
                                      .format    = descriptor.format,
                                      .usage     = descriptor.usage,
                                  });

    const auto image = createImage(device, descriptor.imageType, descriptor.width,
                                   descriptor.height, descriptor.depth, descriptor.mipLevels,
                                   descriptor.format, descriptor.usage, descriptor.samples);

    const VkImageViewType viewType =
        descriptor.imageType == VK_IMAGE_TYPE_3D ? VK_IMAGE_VIEW_TYPE_3D :
        descriptor.imageType == VK_IMAGE_TYPE_2D ? VK_IMAGE_VIEW_TYPE_2D :
                                                   VK_IMAGE_VIEW_TYPE_1D;

    const auto bpp = bytesPerPixelFromFormat(descriptor.format);

    return {
        .image  = image,
        .memory = allocateMemory(device, physicalDevice, image),
        .view   = createImageView(device, image, viewType, descriptor.format, descriptor.aspectMask,
                                  descriptor.mipLevels),
        .sampler       = descriptor.sample ? createSampler(device, {}) : VK_NULL_HANDLE,
        .stagingBuffer = createBuffer(
            device, physicalDevice,
            BufferDescriptor{
                .debugName = "stagingBuffer",
                .byteSize  = descriptor.width * descriptor.height * descriptor.depth * bpp,
                .usage     = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .memoryProperties =
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
                .map = true,
            }),
        .width     = descriptor.width,
        .height    = descriptor.height,
        .depth     = descriptor.depth,
        .bpp       = bpp,
        .format    = descriptor.format,
        .mipLevels = descriptor.mipLevels,
    };
}

static void generateMipmaps(const VkCommandBuffer& commandBuffer, const Texture& texture)
{
    transitionImageLayout(commandBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    for (int32_t i = 1; i < texture.mipLevels; i++)
    {
        VkImageBlit imageBlit               = {};

        // Source
        imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.srcSubresource.layerCount = 1;
        imageBlit.srcSubresource.mipLevel   = i - 1;
        imageBlit.srcOffsets[1].x           = static_cast<int32_t>(texture.width >> (i - 1));
        imageBlit.srcOffsets[1].y           = static_cast<int32_t>(texture.height >> (i - 1));
        imageBlit.srcOffsets[1].z           = 1;

        // Destination
        imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBlit.dstSubresource.layerCount = 1;
        imageBlit.dstSubresource.mipLevel   = i;
        imageBlit.dstOffsets[1].x           = static_cast<int32_t>(texture.width >> i);
        imageBlit.dstOffsets[1].y           = static_cast<int32_t>(texture.height >> i);
        imageBlit.dstOffsets[1].z           = 1;

        transitionImageLayout(commandBuffer, texture.image, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i, 1);

        vkCmdBlitImage(commandBuffer,
                       texture.image,
                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       texture.image,
                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1,
                       &imageBlit,
                       VK_FILTER_LINEAR);

        transitionImageLayout(commandBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i, 1);
    }

    transitionImageLayout(commandBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, texture.mipLevels);
}

void updateTexture(const VkCommandBuffer& commandBuffer, const Texture& texture, const void* data,
                   const bool generateMips)
{
    if (data == nullptr)
        return;

    transitionImageLayout(commandBuffer, texture.image, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    memcpy(texture.stagingBuffer.hostPtr, data, texture.stagingBuffer.byteSize);

    copyBufferToImage(commandBuffer, texture.stagingBuffer.vkBuffer, texture.image,
                      {texture.width, texture.height, texture.depth});

    if (generateMips && texture.mipLevels > 1)
    {
        generateMipmaps(commandBuffer, texture);
    }
    else
    {
        transitionImageLayout(commandBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}

void destroyTexture(const VkDevice& device, const Texture& texture)
{
    vkDestroyImage(device, texture.image, nullptr);
    vkDestroyImageView(device, texture.view, nullptr);
    vkFreeMemory(device, texture.memory, nullptr);
    vkDestroySampler(device, texture.sampler, nullptr);

    destroyBuffer(device, texture.stagingBuffer);
}

VkSampler createSampler(const VkDevice& device, const SamplerDescriptor&& descriptor)
{
    VkSampler sampler                         = VK_NULL_HANDLE;

    VkSamplerCreateInfo samplerCreateInfo     = {};
    samplerCreateInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerCreateInfo.magFilter               = descriptor.magFilter;
    samplerCreateInfo.minFilter               = descriptor.minFilter;
    samplerCreateInfo.addressModeU            = descriptor.addressModeU;
    samplerCreateInfo.addressModeV            = descriptor.addressModeV;
    samplerCreateInfo.addressModeW            = descriptor.addressModeW;
    samplerCreateInfo.anisotropyEnable        = VK_FALSE;
    samplerCreateInfo.maxAnisotropy           = descriptor.maxAnisotropy;
    samplerCreateInfo.borderColor             = descriptor.borderColor;
    samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
    samplerCreateInfo.compareEnable           = descriptor.compareEnable;
    samplerCreateInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
    samplerCreateInfo.mipmapMode              = descriptor.mipmapMode;
    samplerCreateInfo.mipLodBias              = 0.0f;
    samplerCreateInfo.minLod                  = descriptor.minLod;
    samplerCreateInfo.maxLod                  = descriptor.maxLod;

    checkVkResult(vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler));
    return sampler;
}
} // namespace figment::vulkan
