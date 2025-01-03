#pragma once

#include "context.h"
#include "window.h"

#include <imgui.h>
#include <texture.h>

namespace figment::vulkan
{
class ImGuiRenderer
{
public:
    struct Descriptor
    {
        GLFWwindow* window              = nullptr;
        VkInstance instance             = VK_NULL_HANDLE;
        VkDevice device                 = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkQueue queue                   = VK_NULL_HANDLE;
        VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
        uint32_t minImageCount          = 0;
        VkFormat swapchainFormat        = VK_FORMAT_UNDEFINED;
    };

    struct RenderTarget
    {
        VkImage image;
        VkImageView imageView;
        VkRect2D renderArea;
    };
    explicit ImGuiRenderer(const Descriptor&& descriptor);
    ~ImGuiRenderer();

    void begin(const VkCommandBuffer& commandBuffer, const RenderTarget&& target) const;
    void end(const VkCommandBuffer& commandBuffer) const;

private:
    static constexpr bool showDebugInfo               = false;
    PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = nullptr;
    PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR     = nullptr;

    ImGuiContext* imguiContext                        = nullptr;
    VkDevice device                                   = VK_NULL_HANDLE;
};

} // namespace figment::vulkan
