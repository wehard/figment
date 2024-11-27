#pragma once

#include "buffer.h"

#include <vulkan/vulkan.h>

namespace figment::vulkan
{
struct SamplerDescriptor
{
    VkFilter minFilter                = VK_FILTER_NEAREST;
    VkFilter magFilter                = VK_FILTER_NEAREST;
    VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    float maxAnisotropy               = 1.0f;
    VkBorderColor borderColor         = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    VkSamplerMipmapMode mipmapMode    = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    float minLod                      = 0.0f;
    float maxLod                      = 0.0f;
    bool compareEnable                = false;
};

struct TextureDescriptor
{
    const char* debugName   = nullptr;
    VkImageType imageType   = VK_IMAGE_TYPE_2D;
    uint32_t width          = 0;
    uint32_t height         = 0;
    uint32_t depth          = 1;
    VkFormat format         = VK_FORMAT_UNDEFINED;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                              VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    bool sample                   = false;
    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    uint32_t mipLevels            = 1;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
};

struct Texture
{
    VkImage image         = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view      = VK_NULL_HANDLE;
    VkSampler sampler     = VK_NULL_HANDLE;
    Buffer stagingBuffer  = {};
    uint32_t width        = 0;
    uint32_t height       = 0;
    uint32_t depth        = 1;
    uint32_t bpp          = 0;
    VkFormat format       = VK_FORMAT_UNDEFINED;
    uint32_t mipLevels    = 1;
};

Texture createTexture(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                      const TextureDescriptor&& descriptor);

void updateTexture(const VkCommandBuffer& commandBuffer, const Texture& texture, const void* data,
                   bool generateMips = false);

void destroyTexture(const VkDevice& device, const Texture& texture);

VkSampler createSampler(const VkDevice& device, const SamplerDescriptor&& descriptor);

} // namespace figment::vulkan
