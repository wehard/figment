#include "VulkanWindow.h"
#include "Log.h"
#include "FigmentAssert.h"

Figment::VulkanWindow::VulkanWindow(const std::string &title, uint32_t width, uint32_t height) : Window(title, width,
        height)
{
    if (!glfwInit())
    {
        FIGMENT_ASSERT(false, "Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow((int)width, (int)height, "Figment (Vulkan)", nullptr, nullptr);

    if (!m_Window)
    {
        FIGMENT_ASSERT(false, "Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_Window, this);
    // glfwSetWindowSizeCallback(m_Window, glfwWindowSizeCallback);
    // glfwSetFramebufferSizeCallback(m_Window, glfwFramebufferSizeCallback);


    // m_RenderContext = std::make_shared<VulkanContext>();
    // m_RenderContext->Init(m_Width, m_Height);
}
