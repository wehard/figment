#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderContext.h"
#include "vulkan/vulkan.h"
#include "glm/vec3.hpp"
#include <vector>

#define MAX_FRAME_DRAWS 2

namespace Figment
{
    struct SurfaceDetails
    {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentationModes;
    };

    class VulkanContext : public RenderContext
    {
    public:
        VulkanContext(GLFWwindow *window) : m_Window(window) { }
        ~VulkanContext() override;
        void Init(uint32_t width, uint32_t height) override;
        void BeginFrame();
        void EndFrame();
        VkDevice GetDevice() { return m_Device; }
        VkInstance GetInstance() { return m_Instance; }
        VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
        VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

        VkDescriptorPool GetDescriptorPool() { return m_DescriptorPool; }
        SurfaceDetails SurfaceDetails() { return m_SurfaceDetails; }
        VkRenderPass GetRenderPass() { return m_RenderPass; }
        VkCommandBuffer GetCommandBuffer() { return m_CommandBuffers[m_ImageIndex]; }
        VkPipeline GetPipeline() { return m_Pipeline; }
    private:
        GLFWwindow *m_Window;
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;
        VkSwapchainKHR m_SwapChain;
        VkCommandPool m_CommandPool;
        VkRenderPass m_RenderPass;
        VkPipeline m_Pipeline;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<VkFramebuffer> m_SwapChainFramebuffers;

        VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentationQueue;

        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;

        uint32_t m_CurrentFrame = 0;
        uint32_t m_ImageIndex;

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
        void CreateSynchronization();
    };
}
