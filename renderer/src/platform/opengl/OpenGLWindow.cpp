#include "OpenGLWindow.h"
#include <iostream>

static void glfwErrorHandler(int error, const char *message)
{
    std::cout << "Glfw error: " << error << ": " << message << std::endl;
}

OpenGLWindow::OpenGLWindow(const std::string &title, const uint32_t width, const uint32_t height) : m_Title(title), m_Width(width), m_Height(height)
{
    glfwSetErrorCallback(glfwErrorHandler);

    if (!glfwInit())
    {
        std::cout << "GLFW init failed!" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!m_Window)
    {
        glfwTerminate();
        std::cout << "Failed to create GLFW window!" << std::endl;
        exit(EXIT_FAILURE);
    }

    m_GfxContext = GfxContext::Create(m_Window);
    m_GfxContext->Init();

    glfwSwapInterval(0);

    // glfwShowWindow(m_Window);
    int w, h;
    glfwGetWindowSize(m_Window, &w, &h);
    std::cout << "GLFW window created: " << w << " x " << h << std::endl;

    int fw, fh;
    glfwGetFramebufferSize(m_Window, &fw, &fh);
    std::cout << "GLFW framebuffer size: " << fw << " x " << fh << std::endl;
}

OpenGLWindow::~OpenGLWindow()
{
    glfwDestroyWindow(m_Window);
}

void OpenGLWindow::Resize(uint32_t width, uint32_t height)
{
    m_Width = width;
    m_Height = height;
    glfwSetWindowSize(m_Window, width, height);
}

uint32_t OpenGLWindow::GetWidth() const
{
    return m_Width;
}

uint32_t OpenGLWindow::GetHeight() const
{
    return m_Height;
}
