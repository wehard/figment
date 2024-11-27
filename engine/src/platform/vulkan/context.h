#pragma once

#include "vulkan/vulkan.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "DeletionQueue.h"
#include "RenderContext.h"

#include "glm/vec3.hpp"

#include <vector>

namespace figment::vulkan
{
class VulkanSwapchain;
class Context: public RenderContext
{
public:
    struct VulkanSurfaceDetails
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentationModes;
    };

    [[nodiscard]] VkExtent2D getSwapchainExtent() const;
    [[nodiscard]] VulkanSwapchain* getSwapchain() const;

public:
    explicit Context(GLFWwindow* window): m_Window(window) {}
    ~Context() override;
    void Init(uint32_t width, uint32_t height) override;

    [[nodiscard]] VkInstance GetInstance() const { return m_Instance; }
    [[nodiscard]] VkDevice GetDevice() const { return m_Device; };
    [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
    [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
    [[nodiscard]] VulkanSurfaceDetails SurfaceDetails() const { return m_SurfaceDetails; }
    [[nodiscard]] VkDescriptorPool createDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes,
                                                        uint32_t maxSets) const;
    [[nodiscard]] VkCommandPool createCommandPool() const;
    [[nodiscard]] uint32_t getSwapchainImageCount() const;
    [[nodiscard]] std::vector<VkImageView> getSwapchainImageViews() const;

    void onResize(uint32_t width, uint32_t height) override;
    [[nodiscard]] VkCommandBuffer beginSingleTimeCommands() const;
    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;
    void cleanup();

    [[nodiscard]] uint32_t findMemoryTypeIndex(uint32_t allowedTypes,
                                               VkMemoryPropertyFlags properties) const;

    PFN_vkCmdBeginRenderingKHR vkCmdBeginRenderingKHR = nullptr;
    PFN_vkCmdEndRenderingKHR vkCmdEndRenderingKHR     = nullptr;

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Color;
    };

private:
    GLFWwindow* m_Window;
    VkInstance m_Instance             = VK_NULL_HANDLE;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_Device                 = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface            = VK_NULL_HANDLE;
    VulkanSwapchain* m_Swapchain      = nullptr;

    DeletionQueue m_DeletionQueue;

    VkPipelineCache m_PipelineCache       = VK_NULL_HANDLE;

    uint32_t m_GraphicsQueueIndex         = 0;
    VkQueue m_GraphicsQueue               = VK_NULL_HANDLE;

    VulkanSurfaceDetails m_SurfaceDetails = {};
    VkCommandPool m_SingleTimeCommandPool = VK_NULL_HANDLE;

    void createInstance(const VkApplicationInfo& applicationInfo);
    void createSurface();
    void createDevice();
    void createSwapchain();
    void createPipelineCache();

    void cleanupSwapchain() const;
    void recreateSwapchain();
};
} // namespace figment::vulkan
