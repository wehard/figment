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

#define MAX_FRAME_DRAWS 2

namespace Figment
{
    class VulkanBuffer;
    class VulkanShader;
    class VulkanPipeline;
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

        struct UniformBufferObject
        {
            glm::mat4 Model;
            glm::mat4 View;
            glm::mat4 Projection;
        };

        VkFramebuffer GetCurrentFramebuffer() const;
        VkCommandBuffer GetCurrentCommandBuffer() const;
        VulkanBuffer *GetCurrentUniformBuffer() const;
        VulkanBindGroup *GetCurrentBindGroup() const;
    public:
        explicit VulkanContext(GLFWwindow *window) : m_Window(window) { }
        ~VulkanContext() override;
        void Init(uint32_t width, uint32_t height) override;

        [[nodiscard]] VkInstance GetInstance() const { return m_Instance; }
        [[nodiscard]] VkDevice GetDevice() const { return m_Device; };
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevice; }
        [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
        [[nodiscard]] VkQueue GetPresentQueue() const { return m_GraphicsQueue; }
        [[nodiscard]] VkSurfaceKHR GetSurface() const { return m_Surface; }
        [[nodiscard]] VkCommandPool GetImGuiCommandPool() const { return m_CommandPool; }
        [[nodiscard]] VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }
        [[nodiscard]] VulkanSurfaceDetails SurfaceDetails() const { return m_SurfaceDetails; }
        // [[nodiscard]] VkRenderPass GetRenderPass() const { return m_RenderPass; }
        [[nodiscard]] VkCommandBuffer GetImGuiCommandBuffer() const { return m_ImGuiCommandBuffers[m_FrameIndex]; }
        // [[nodiscard]] VkPipeline GetPipeline() const { return m_Pipeline; }
        [[nodiscard]] VkFramebuffer GetImGuiFramebuffer() const { return m_FrameData.ImGuiFramebuffers[m_ImageIndex]; }
        [[nodiscard]] VkImageView GetCurrentImageView() const { return m_FrameData.ImageViews[m_ImageIndex]; }
        [[nodiscard]]VkDescriptorPool CreateDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes,
                uint32_t maxSets);

        VulkanRenderPass *GetImGuiRenderPass() { return m_ImGuiRenderPass; }

        void OnResize(uint32_t width, uint32_t height) override;
        void BeginFrame();
        void EndFrame();
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
        // VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        VulkanRenderPass *m_RenderPass = nullptr;
        VulkanSwapchain *m_Swapchain = nullptr;

        DeletionQueue m_DeletionQueue;

        VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        uint32_t m_GraphicsQueueIndex = UINT32_MAX;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;

        uint32_t m_FrameIndex = 0;
        uint32_t m_ImageIndex = 0;

        VulkanPipeline *m_Pipeline = nullptr;
        VulkanShader *m_Shader = nullptr;
        VulkanBuffer *m_Buffer = nullptr;

        VulkanSurfaceDetails m_SurfaceDetails = {};

        const std::vector<const char *> m_RequiredDeviceExtensions = {
                "VK_KHR_swapchain",
                "VK_KHR_portability_subset"
        };

        struct SynchronizationObjects
        {
            VkSemaphore SemaphoreImageAvailable;
            VkSemaphore SemaphoreRenderFinished;
            VkFence FenceDraw;
        };

        struct FrameData
        {
            std::vector<VkFramebuffer> Framebuffers;
            std::vector<VkFramebuffer> ImGuiFramebuffers;
            std::vector<VkImage> Images;
            std::vector<VkImageView> ImageViews;

            void Init(uint32_t size)
            {
                Framebuffers.resize(size);
                ImGuiFramebuffers.resize(size);
                Images.resize(size);
                ImageViews.resize(size);
            }
        };

        FrameData m_FrameData;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<SynchronizationObjects> m_SynchronizationObjects;
        std::vector<VulkanBindGroup *> m_BindGroups;
        std::vector<VulkanBuffer *> m_UniformBuffers;

        VulkanRenderPass *m_ImGuiRenderPass = nullptr;
        std::vector<VkCommandBuffer> m_ImGuiCommandBuffers;
        VkCommandPool m_ImGuiCommandPool = VK_NULL_HANDLE;

        void CreateInstance();
        void CreateSurface();
        void CreateDevice();
        void CreateSwapchain();
        void CreateRenderPass();
        void CreateImGuiRenderPass();
        void CreatePipeline(VkShaderModule vertexModule, VkShaderModule fragmentModule);
        void CreatePipelineCache();
        void CreateDescriptorSets();
        void CreateCommandPool();
        void CreateImGuiCommandPool();
        void CreateCommandBuffers();
        void CreateImGuiCommandBuffers();
        void CreateFramebuffers();
        void CreateImGuiFramebuffers();
        void CreateSynchronization();

        void CleanupSwapchain();
        void RecreateSwapchain();
    };
}
