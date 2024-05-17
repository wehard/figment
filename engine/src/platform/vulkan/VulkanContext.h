#pragma once

#include "RenderContext.h"
#include "vulkan/vulkan.h"

namespace Figment
{
    class VulkanContext : public RenderContext
    {
    public:
        ~VulkanContext() override;
        void Init(uint32_t width, uint32_t height) override;
    private:
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
    };
}
