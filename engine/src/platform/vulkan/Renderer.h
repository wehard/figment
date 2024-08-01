#pragma once

#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanBuffer.h"
#include "VulkanBindGroup.h"

namespace Figment::Vulkan
{
    class Renderer
    {
    public:
        Renderer() = delete;
        explicit Renderer(const VulkanContext &context);
        ~Renderer();

        void Begin(const Camera &camera);
        void End();

        void Draw(VulkanBuffer &buffer, glm::mat4 transform, Camera &camera);

        void OnResize(uint32_t width, uint32_t height);
        [[nodiscard]] VkRenderPass GetGuiRenderPass() const { return m_GuiPass->Get(); }
        [[nodiscard]] VkFramebuffer GetCurrentGuiFramebuffer() const { return m_GuiFramebuffers[m_Context.GetSwapchainImageIndex()]; }
    private:
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Color;
        };
    private:
        const VulkanContext &m_Context;
        std::unique_ptr<VulkanRenderPass> m_OpaquePass = nullptr;
        std::unique_ptr<VulkanPipeline> m_OpaquePipeline = nullptr;
        std::unique_ptr<VulkanShader> m_Shader = nullptr;
        std::vector<VkFramebuffer> m_Framebuffers;

        std::unique_ptr<VulkanRenderPass> m_GuiPass = nullptr;
        std::unique_ptr<VulkanPipeline> m_GuiPipeline = nullptr;
        std::vector<VkFramebuffer> m_GuiFramebuffers;
    private:
        void CreateRenderPass();
        void CreatePipeline();
        void CreateFramebuffers();

        void CreateGuiRenderPass();
        void CreateGuiPipeline();
        void CreateGuiFramebuffers();
    };
}
