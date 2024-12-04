#pragma once
#define GLFW_INCLUDE_VULKAN
#include "imgui_renderer.h"

#include <GLFW/glfw3.h>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace vk_imgui_presenter
{
inline void checkVkResult(const VkResult result)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Vulkan error: " + std::to_string(result));
    }
}

class VkImGuiPresenter
{
public:
    VkImGuiPresenter();

    ~VkImGuiPresenter();

    void init(const VkInstance& instance, const VkDevice& device,
              const VkPhysicalDevice& physicalDevice, const VkQueue& queue, uint32_t queueIndex,
              uint32_t width, uint32_t height);

    void createCommandBuffers(const VkDevice& device, uint32_t graphicsQueueIndex);

    void createWindow(VkExtent2D extent);

    void createSurface(const VkInstance& instance);

    void createSwapchain(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
                         VkExtent2D extent);

    void createSyncObjects(const VkDevice& device);

    void getNextImageIndex(const VkDevice& device, const VkSemaphore& semaphore)
    {
        checkVkResult(vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(),
                                            semaphore, nullptr, &imageIndex));
    }

    void prepare(const VkDevice& device);

    void present(const VkQueue& queue);

    void cleanup(const VkInstance& instance, const VkDevice& device);

    static void addInstanceExtensions(std::vector<const char*>& instanceExtensions)
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (size_t i = 0; i < glfwExtensionCount; i++)
        {
            instanceExtensions.push_back(glfwExtensions[i]);
        }
    }

private:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    GLFWwindow* window                        = nullptr;
    VkExtent2D windowExtent                   = {0, 0};
    VkSurfaceKHR surface                      = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain                  = VK_NULL_HANDLE;
    uint32_t imageCount                       = 0;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    uint32_t imageIndex                                 = 0;
    std::vector<VkSemaphore> imageAvailableSemaphores   = {};
    std::vector<VkSemaphore> renderFinishedSemaphores   = {};
    VkCommandPool commandPool                           = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> commandBuffers         = {};
    std::vector<VkFence> fences                         = {};
    uint32_t frameIndex                                 = 0;
    VkSurfaceFormatKHR surfaceFormat                    = {};
    ImGuiRenderer* imguiRenderer                        = nullptr;

    PFN_vkGetSwapchainStatusKHR vkGetSwapchainStatusKHR = nullptr;
};
}; // namespace vk_imgui_presenter
