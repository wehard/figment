#include "OpenGLWindow.h"
#include <iostream>

namespace Figment
{
    static void glfwErrorHandler(int error, const char *message)
    {
        std::cout << "Glfw error: " << error << ": " << message << std::endl;
    }

    static void glfwWindowSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *openGlWindow = (OpenGLWindow *)glfwGetWindowUserPointer(window);
        openGlWindow->Resize({
                .Width = width,
                .Height = height,
                .FramebufferWidth = static_cast<int>(openGlWindow->GetFramebufferWidth()),
                .FramebufferHeight = static_cast<int>(openGlWindow->GetFramebufferHeight()) });
    }

    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *openGlWindow = (OpenGLWindow *)glfwGetWindowUserPointer(window);
        openGlWindow->Resize({
                .Width = static_cast<int>(openGlWindow->GetWidth()),
                .Height = static_cast<int>(openGlWindow->GetHeight()),
                .FramebufferWidth = width,
                .FramebufferHeight = height });
    }

    OpenGLWindow::OpenGLWindow(const std::string &title, const uint32_t width, const uint32_t height) : m_Title(title),
            m_Width(width), m_Height(height)
    {

        if (!glfwInit())
        {
            std::cout << "GLFW init failed!" << std::endl;
            return;
        }

        glfwSetErrorCallback(glfwErrorHandler);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
        glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (!m_Window)
        {
            glfwTerminate();
            std::cout << "Failed to create GLFW window!" << std::endl;
            exit(EXIT_FAILURE);
        }

        m_GfxContext = GfxContext::Create(m_Window);
        m_GfxContext->Init();

        int w, h;
        glfwGetWindowSize(m_Window, &w, &h);
        std::cout << "GLFW window created: " << w << " x " << h << std::endl;

        int fw, fh;
        glfwGetFramebufferSize(m_Window, &fw, &fh);
        std::cout << "GLFW framebuffer size: " << fw << " x " << fh << std::endl;
        m_FramebufferWidth = (uint32_t)fw;
        m_FramebufferHeight = (uint32_t)fh;

        glfwSetWindowSizeCallback(m_Window, glfwWindowSizeCallback);
        glfwSetFramebufferSizeCallback(m_Window, glfwFramebufferSizeCallback);

        glfwSetWindowUserPointer(m_Window, this);

    }

    OpenGLWindow::~OpenGLWindow()
    {
        glfwDestroyWindow(m_Window);
    }

    void OpenGLWindow::Resize(WindowResizeEventData resizeData)
    {
        glViewport(0, 0, resizeData.FramebufferWidth, resizeData.FramebufferHeight);
        m_Width = resizeData.Width;
        m_Height = resizeData.Height;
        m_FramebufferWidth = resizeData.FramebufferWidth;
        m_FramebufferHeight = resizeData.FramebufferHeight;

        if (ResizeEventCallback != nullptr)
        {
            ResizeEventCallback(resizeData);
        }
    }

    bool OpenGLWindow::ShouldClose()
    {
        return glfwWindowShouldClose(m_Window);
    }

    uint32_t OpenGLWindow::GetWidth() const
    {
        return m_Width;
    }

    uint32_t OpenGLWindow::GetHeight() const
    {
        return m_Height;
    }

    uint32_t OpenGLWindow::GetFramebufferWidth() const
    {
        return m_FramebufferWidth;
    }

    uint32_t OpenGLWindow::GetFramebufferHeight() const
    {
        return m_FramebufferHeight;
    }

    void OpenGLWindow::SetResizeEventCallback(ResizeEventCallbackFn callback)
    {
        ResizeEventCallback = callback;
    }
}