#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>

#include "RenderContext.h"
#include "Log.h"
#include "Camera.h"
#include "DeletionQueue.h"

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"

#include <vector>

namespace Figment
{
    class VulkanBuffer;
    class VulkanShader;
    class VulkanRenderPass;
    class VulkanBindGroup;
    class VulkanSwapchain;

    inline void CheckVkResult(VkResult result)
    {
        if (result == 0)
            return;
        FIG_LOG_ERROR("[Vulkan] Error: VkResult = %d\n", result);
        if (result < 0)
            abort();
    }

    class VulkanContext : public RenderContext
    {
    public:
        struct VulkanSurfaceDetails
        {
            VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentationModes;
        };

        [[nodiscard]] VkExtent2D GetSwapchainExtent() const;
        [[nodiscard]] VulkanSwapchain *GetSwapchain() const;
    public:
        explicit VulkanContext(GLFWwindow *window) : m_Window(window) { }
        ~VulkanContext() override;
        void Init(uint32_t width, uint32_t height) override;

        [[nodiscard]] VkInstance GetInstance() const { return m_Instance; }
        [[nodiscard]] VkDevice GetDevice() const { return m_Device; };
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        [[nodiscard]] VulkanSurfaceDetails SurfaceDetails() const { return m_SurfaceDetails; }
        [[nodiscard]] VkDescriptorPool CreateDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes,
                uint32_t maxSets) const;
        [[nodiscard]] VkCommandPool CreateCommandPool() const;
        [[nodiscard]] uint32_t GetSwapchainImageCount() const;
        [[nodiscard]] std::vector<VkImageView> GetSwapchainImageViews() const;

        void OnResize(uint32_t width, uint32_t height) override;
        [[nodiscard]] VkCommandBuffer BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;
        void Cleanup();

        [[nodiscard]] uint32_t FindMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties) const;

        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };

    private:
        GLFWwindow *m_Window;
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VulkanSwapchain *m_Swapchain = nullptr;

        DeletionQueue m_DeletionQueue;

        VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

        uint32_t m_GraphicsQueueIndex = UINT32_MAX;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        VulkanSurfaceDetails m_SurfaceDetails = {};

        const std::vector<const char *> m_RequiredDeviceExtensions = {
                "VK_KHR_swapchain",
                "VK_KHR_portability_subset"
        };

        VkCommandPool m_SingleTimeCommandPool = VK_NULL_HANDLE;

        void CreateInstance();
        void CreateSurface();
        void CreateDevice();
        void CreateSwapchain();
        void CreatePipelineCache();

        void CleanupSwapchain();
        void RecreateSwapchain();
    };
}
