#pragma once

#include <vulkan/vulkan.h>

namespace figment::vulkan
{
void transitionImageLayout(const VkCommandBuffer& commandBuffer, const VkImage& image,
                           VkImageLayout oldLayout, VkImageLayout newLayout,
                           uint32_t baseMipLevel = 0, uint32_t mipLevels = 1);
} // namespace figment::vulkan
