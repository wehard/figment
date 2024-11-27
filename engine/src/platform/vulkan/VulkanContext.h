#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "DeletionQueue.h"
#include "RenderContext.h"

#include "glm/vec3.hpp"

#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace Figment
{
class VulkanBuffer;
class VulkanShader;
class VulkanRenderPass;
class VulkanBindGroup;
class VulkanSwapchain;

static std::string vkResultToString(VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:
            return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:
            return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        default:
            return "UNKNOWN_ERROR";
    }
}

inline void checkVkResult(const VkResult result)
{
    if (result == VK_SUCCESS)
        return;
    spdlog::error("Vulkan error: VkResult = {}", vkResultToString(result));
}

class VulkanContext: public RenderContext
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
    explicit VulkanContext(GLFWwindow* window): m_Window(window) {}
    ~VulkanContext() override;
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
} // namespace Figment
