#pragma once

#include "vulkan/vulkan.h"
#include <GLFW/glfw3.h>
#include "RenderContext.h"
#include "glm/vec3.hpp"
#include <vector>

#define MAX_FRAME_DRAWS 2

namespace Figment
{
    class VulkanBuffer;

    struct SurfaceDetails
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentationModes;
    };

    class VulkanContext : public RenderContext
    {
    public:
        explicit VulkanContext(GLFWwindow *window) : m_Window(window) { }
        ~VulkanContext() override;
        void Init(uint32_t width, uint32_t height) override;
        void DebugDraw();

        VkDevice GetDevice() { return m_Device; }
        VkInstance GetInstance() { return m_Instance; }
        VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
        // VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

        VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }
        SurfaceDetails SurfaceDetails() { return m_SurfaceDetails; }
        VkRenderPass GetRenderPass() { return m_RenderPass; }
        VkCommandBuffer GetCommandBuffer() { return m_CommandBuffers[m_ImageIndex]; }
        VkPipeline GetPipeline() { return m_Pipeline; }
        VkFramebuffer GetFramebuffer() { return m_SwapChainFramebuffers[m_ImageIndex]; }
    private:
        GLFWwindow *m_Window;
        VkInstance m_Instance = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VkRenderPass m_RenderPass = VK_NULL_HANDLE;
        VkPipeline m_Pipeline = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkFramebuffer> m_SwapChainFramebuffers;

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

        VulkanBuffer *m_Buffer = nullptr;

        struct SurfaceDetails m_SurfaceDetails;

        const std::vector<const char *> m_RequiredDeviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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

        std::vector<SwapChainImage> m_SwapChainImages;

        // Sync objects
        std::vector<VkSemaphore> m_ImageAvailable;
        std::vector<VkSemaphore> m_RenderFinished;
        std::vector<VkFence> m_DrawFences;

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
    };
}
