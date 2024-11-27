#include "window.h"
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

Window::Window(const std::string& title, const uint32_t width, const uint32_t height):
    figment::Window(title, width, height)
{
    if (!glfwInit())
    {
        spdlog::error("Failed to initialize GLFW");
    }

    if (glfwVulkanSupported())
    {
        spdlog::info("Vulkan is supported");
    }
    else
    {
        spdlog::error("Vulkan is not supported");
    }

    spdlog::info("GLFW version: {}", glfwGetVersionString());

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(),
                              nullptr, nullptr);

    if (!window)
    {
        FIGMENT_ASSERT(false, "Failed to create GLFW window");
    }

    glfwShowWindow(window);
    glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
    glfwSetWindowUserPointer(window, this);

    m_RenderContext = std::make_shared<Context>(window);
    m_RenderContext->Init(m_Width, m_Height);

    // Create vulkan surface
    // VkResult result = glfwCreateWindowSurface(instance, m_Window, nullptr, &surface);

    spdlog::info("Vulkan window created");
}
} // namespace figment::vulkan
