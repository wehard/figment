#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include "RenderContext.h"
#include "glm/vec3.hpp"
#include "Log.h"
#include <vector>

#define MAX_FRAME_DRAWS 2

namespace Figment
{
    class VulkanBuffer;
    class VulkanShader;
    class VulkanPipeline;

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

        explicit VulkanContext(GLFWwindow *window) : m_Window(window) { }
        ~VulkanContext() override;
        void Init(uint32_t width, uint32_t height) override;
        void DebugDraw();

        [[nodiscard]] VkInstance GetInstance() const { return m_Instance; }
        [[nodiscard]] VkDevice GetDevice() const { return m_Device; };
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        [[nodiscard]] VkQueue GetPresentQueue() const { return m_GraphicsQueue; }
        [[nodiscard]] VkSurfaceKHR GetSurface() const { return m_Surface; }
        [[nodiscard]] VkSwapchainKHR GetSwapChain() const { return m_SwapChain; }
        [[nodiscard]] VkCommandPool GetCommandPool() const { return m_CommandPool; }
        [[nodiscard]] VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }
        [[nodiscard]] VulkanSurfaceDetails SurfaceDetails() const { return m_SurfaceDetails; }
        [[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass; }
        [[nodiscard]] VkCommandBuffer GetCommandBuffer() const { return m_CommandBuffers[m_ImageIndex]; }
        // [[nodiscard]] VkPipeline GetPipeline() const { return m_Pipeline; }
        [[nodiscard]] VkFramebuffer GetFramebuffer() const { return m_SwapChainFramebuffers[m_ImageIndex]; }

        void OnResize(uint32_t width, uint32_t height) override;
    private:
        GLFWwindow *m_Window;
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;

        VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        uint32_t m_GraphicsQueueIndex = UINT32_MAX;
        uint32_t m_PresentQueueIndex = UINT32_MAX;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        // VkQueue m_PresentationQueue = VK_NULL_HANDLE;

        VkFormat m_SwapChainImageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D m_SwapChainExtent = { 0, 0 };

        uint32_t m_CurrentFrame = 0;
        uint32_t m_ImageIndex = 0;

        VulkanPipeline *m_Pipeline = nullptr;
        VulkanShader *m_Shader = nullptr;
        VulkanBuffer *m_Buffer = nullptr;

        VulkanSurfaceDetails m_SurfaceDetails = {};

        const std::vector<const char *> m_RequiredDeviceExtensions = {
                "VK_KHR_swapchain",
                "VK_KHR_portability_subset"
        };

        struct SwapChainImage
        {
            VkImage image;
            VkImageView imageView;
        };

        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };

        struct SynchronizationObjects
        {
            VkSemaphore SemaphoreImageAvailable;
            VkSemaphore SemaphoreRenderFinished;
            VkFence FenceDraw;
        };

        SynchronizationObjects m_SynchronizationObjects[MAX_FRAME_DRAWS];

        struct FrameData
        {
            VkCommandBuffer CommandBuffer;
            VkFramebuffer FrameBuffer;
            VkImage Image;
            VkImageView ImageView;
        };

        std::vector<SwapChainImage> m_SwapChainImages;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkFramebuffer> m_SwapChainFramebuffers;

        void CreateInstance();
        void CreateSurface();
        void CreateDevice();
        void CreateSwapChain();
        void CreateRenderPass();
        void CreatePipeline(VkShaderModule vertexModule, VkShaderModule fragmentModule);
        void CreatePipelineCache();
        void CreateDescriptorPool();
        void CreateCommandPool();
        void CreateCommandBuffers();
        void CreateFramebuffers();
        void RecordCommands();
        void CreateSynchronization();

        void CleanupSwapChain();
        void RecreateSwapChain();
    };
}
