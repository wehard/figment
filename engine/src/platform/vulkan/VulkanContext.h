#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderContext.h"
#include "vulkan/vulkan.h"
#include <vector>

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
        VkDevice GetDevice() { return m_Device; }
        VkInstance GetInstance() { return m_Instance; }
        VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevice; }
        VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
    private:
        GLFWwindow *m_Window;
        VkInstance m_Instance;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkSurfaceKHR m_Surface;
        VkSwapchainKHR m_SwapChain;

        VkQueue m_GraphicsQueue;
        VkQueue m_PresentationQueue;

        const std::vector<const char *> m_RequiredDeviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                "VK_KHR_portability_subset"
        };

        void CreateInstance();
        void CreateSurface();
        void CreateDevice();
        void CreateSwapChain();
    };
}
