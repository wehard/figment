#include "VulkanWindow.h"
#include "FigmentAssert.h"
#include "context.h"
#include <spdlog/spdlog.h>

namespace figment::vulkan
{
static void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* figmentWindow = (Window*)glfwGetWindowUserPointer(window);
    figmentWindow->OnResize(
        {.Width = width, .Height = height, .FramebufferWidth = width, .FramebufferHeight = height});
}

VulkanWindow::VulkanWindow(const std::string& title, uint32_t width, uint32_t height):
    Window(title, width, height)
{
    if (!glfwInit())
    {
        FIGMENT_ASSERT(false, "Failed to initialize GLFW");
    }

    if (glfwVulkanSupported())
    {
        spdlog::info("Vulkan is supported");
    }
    else
    {
        FIGMENT_ASSERT(false, "Vulkan is not supported");
    }

    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    spdlog::info("GLFW version: {}", glfwGetVersionString());

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

    m_RenderContext = std::make_shared<Context>(m_Window);
    m_RenderContext->Init(m_Width, m_Height);

    // Create vulkan surface
    // VkResult result = glfwCreateWindowSurface(instance, m_Window, nullptr, &surface);

    spdlog::info("Vulkan window created");
}
} // namespace figment::vulkan
