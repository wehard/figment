#include "VulkanWindow.h"
#include "Log.h"
#include "FigmentAssert.h"
#include "VulkanContext.h"

namespace Figment
{
    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *figmentWindow = (Window *)glfwGetWindowUserPointer(window);
        figmentWindow->OnResize({
                .Width = width,
                .Height = height,
                .FramebufferWidth = width,
                .FramebufferHeight = height });
    }

    Figment::VulkanWindow::VulkanWindow(const std::string &title, uint32_t width, uint32_t height) : Window(title,
            width,
            height)
    {
        if (!glfwInit())
        {
            FIGMENT_ASSERT(false, "Failed to initialize GLFW");
        }

        if (glfwVulkanSupported())
        {
            FIG_LOG_INFO("Vulkan is supported");
        }
        else
        {
            FIGMENT_ASSERT(false, "Vulkan is not supported");
        }

        uint32_t count;
        const char **extensions = glfwGetRequiredInstanceExtensions(&count);

        FIG_LOG_INFO("GLFW version: %s", glfwGetVersionString());

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_Window = glfwCreateWindow((int)width, (int)height, title.c_str(), nullptr, nullptr);

        if (!m_Window)
        {
            FIGMENT_ASSERT(false, "Failed to create GLFW window");
        }

        glfwShowWindow(m_Window);
        glfwSetFramebufferSizeCallback(m_Window, glfwFramebufferSizeCallback);
        glfwSetWindowUserPointer(m_Window, this);

        m_RenderContext = std::make_shared<VulkanContext>(m_Window);
        m_RenderContext->Init(m_Width, m_Height);


        // Create vulkan surface
        // VkResult result = glfwCreateWindowSurface(instance, m_Window, nullptr, &surface);

        FIG_LOG_INFO("Vulkan window created");
    }
}
