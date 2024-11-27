#pragma once

#include "VulkanContext.h"

namespace figment
{
struct VulkanTextureDescriptor
{
    int Width       = 0;
    int Height      = 0;
    int Channels    = 4;
    void* Data      = nullptr;
    VkFormat Format = VK_FORMAT_R8G8B8A8_SRGB;
};

class VulkanTexture
{
public:
    explicit VulkanTexture(const VulkanContext& context,
                           const VulkanTextureDescriptor&& descriptor);
    ~VulkanTexture() = default;

    glm::vec4 GetPixel(int x, int y) const;

    [[nodiscard]] VkImageView GetImageView() const { return m_ImageView; }
    [[nodiscard]] VkSampler GetSampler() const { return m_Sampler; }

private:
    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) const;
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

private:
    const VulkanContext& m_Context;

    VkImage m_Image              = VK_NULL_HANDLE;
    VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
    VkImageView m_ImageView      = VK_NULL_HANDLE;
    VkSampler m_Sampler          = VK_NULL_HANDLE;
};
} // namespace figment
