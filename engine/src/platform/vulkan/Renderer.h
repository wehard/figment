#pragma once

#include "VulkanContext.h"
#include "VulkanRenderPass.h"

namespace Figment::Vulkan
{
    class Renderer
    {
    public:
        Renderer() = delete;
        explicit Renderer(const VulkanContext &context);
        ~Renderer();
    private:
        const VulkanContext &m_Context;
        std::unique_ptr<VulkanRenderPass> m_OpaquePass = nullptr;
    };
}
