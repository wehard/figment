#include "VulkanWindow.h"
#include "Log.h"
#include "FigmentAssert.h"
#include "VulkanContext.h"

Figment::VulkanWindow::VulkanWindow(const std::string &title, uint32_t width, uint32_t height) : Window(title, width,
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow((int)width, (int)height, "Figment (Vulkan)", nullptr, nullptr);

    if (!m_Window)
    {
        FIGMENT_ASSERT(false, "Failed to create GLFW window");
    }

    glfwShowWindow(m_Window);
    glfwSetWindowUserPointer(m_Window, this);

    m_RenderContext = std::make_shared<VulkanContext>(m_Window);
    m_RenderContext->Init(m_Width, m_Height);


    // Create vulkan surface
    // VkResult result = glfwCreateWindowSurface(instance, m_Window, nullptr, &surface);

    FIG_LOG_INFO("Vulkan window created");
}
