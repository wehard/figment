#pragma once

#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanBindGroup.h"
#include "ResourceManager.h"

#include "imgui.h"

namespace Figment::Vulkan
{
    class Renderer
    {
    public:
        Renderer() = delete;
        explicit Renderer(const VulkanContext &context);
        ~Renderer() = default;
        void Init();
        void InitGui(GLFWwindow *window);
        void Shutdown();
        void BeginFrame();
        void EndFrame();
        void Begin(const Camera &camera);
        void End();

        void BeginGuiPass();
        void EndGuiPass();

        void Draw(VulkanBuffer &buffer, glm::mat4 transform, Camera &camera);

        void OnResize(uint32_t width, uint32_t height);
        [[nodiscard]] VkRenderPass GetGuiRenderPass() const { return m_GuiPass->Get(); }
        [[nodiscard]] VkFramebuffer GetCurrentGuiFramebuffer() const { return m_GuiFramebuffers[m_ImageIndex]; }
        [[nodiscard]] VkCommandBuffer GetGuiCommandBuffer() const;
    private:
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };

        struct GlobalUniformData
        {
            glm::mat4 Model;
            glm::mat4 View;
            glm::mat4 Projection;
        };

        struct PushConstantData
        {
            glm::mat4 Model;
        };

        struct SynchronizationObjects
        {
            VkSemaphore SemaphoreImageAvailable;
            VkSemaphore SemaphoreRenderFinished;
            VkFence FenceDraw;
        };

        struct FrameResource
        {
            VulkanBindGroup *BindGroup = nullptr;
            GlobalUniformData GlobalUniformData = {};
            VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
            VkCommandBuffer GuiCommandBuffer = VK_NULL_HANDLE;
            SynchronizationObjects SynchronizationObjects = {};
        };
    private:
        constexpr static uint32_t MAX_FRAMES_IN_FLIGHT = 2;
        const VulkanContext &m_Context;
        std::vector<SynchronizationObjects> m_SynchronizationObjects;
        std::unique_ptr<VulkanRenderPass> m_OpaquePass = nullptr;
        std::unique_ptr<VulkanPipeline> m_OpaquePipeline = nullptr;
        std::unique_ptr<VulkanShader> m_Shader = nullptr;
        std::vector<VkFramebuffer> m_Framebuffers;

        std::unique_ptr<VulkanRenderPass> m_GuiPass = nullptr;
        std::unique_ptr<VulkanPipeline> m_GuiPipeline = nullptr;
        std::vector<VkFramebuffer> m_GuiFramebuffers;

        std::vector<VulkanBuffer *> m_GlobalUniformBuffers;
        std::vector<VulkanBindGroup *> m_BindGroups;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        uint32_t m_ImageIndex = 0;
        uint32_t m_FrameIndex = 0;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_CommandBuffers;

        VkCommandPool m_GuiCommandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> m_GuiCommandBuffers;

        DeletionQueue m_DeletionQueue;
        FrameResource m_FrameResources[MAX_FRAMES_IN_FLIGHT];

        ImGuiContext *m_GuiContext = nullptr;

        ResourceManager m_ResourceManager;
    private:
        void CreateSynchronizationObjects();

        void CreateRenderPass();
        void CreatePipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffers();

        void CreateGuiRenderPass();
        void CreateGuiPipeline();
        void CreateGuiFramebuffers();
        void CreateGuiCommandPool();
        void CreateGuiCommandBuffers();

        void CreateGlobalUniformBuffers();
        void CreateDescriptorSets();
        void CreateDescriptorPool();
    };
}
