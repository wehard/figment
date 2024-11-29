#pragma once
#include "context.h"

#include <swapchain.h>
#include <vector>
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
class Renderer
{
public:
    struct Data
    {
        VkCommandBuffer commandBuffer;
        VkImage image;
        VkImageView imageView;
    };
    Renderer(const Context& context);
    ~Renderer();

    Data begin(const Swapchain& swapchain);
    void render();
    void end(const Swapchain& swapchain);

    [[nodiscard]] VkSemaphore renderFinishedSemaphore() const
    {
        return renderFinishedSemaphores[frameIndex];
    }

    [[nodiscard]] VkSemaphore imageAvailableSemaphore() const
    {
        return imageAvailableSemaphores[frameIndex];
    }

private:
    constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 2;

    const Context& context;
    VkCommandPool commandPool                         = VK_NULL_HANDLE;
    std::vector<VkSemaphore> renderFinishedSemaphores = {};
    std::vector<VkSemaphore> imageAvailableSemaphores = {};
    std::vector<VkFence> fences                       = {};
    std::vector<VkCommandBuffer> commandBuffers       = {};

    uint32_t frameIndex                               = 0;
    uint32_t imageIndex                               = 0;
};
} // namespace figment::vulkan
