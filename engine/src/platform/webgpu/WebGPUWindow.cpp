#include "WebGPUWindow.h"
#include "Log.h"
#include <iostream>
#include <emscripten/html5.h>

namespace Figment
{
    static void glfwErrorHandler(int error, const char *message)
    {
        std::cout << "Glfw error: " << error << ": " << message << std::endl;
    }

    static void glfwWindowSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *webGpuWindow = (WebGPUWindow *)glfwGetWindowUserPointer(window);
        webGpuWindow->Resize({
                .Width = width,
                .Height = height,
                .FramebufferWidth = static_cast<int>(webGpuWindow->GetFramebufferWidth()),
                .FramebufferHeight = static_cast<int>(webGpuWindow->GetFramebufferHeight()) });
    }

    static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
    {
        auto *webGpuWindow = (WebGPUWindow *)glfwGetWindowUserPointer(window);
        webGpuWindow->Resize({
                .Width = static_cast<int>(webGpuWindow->GetWidth()),
                .Height = static_cast<int>(webGpuWindow->GetHeight()),
                .FramebufferWidth = width,
                .FramebufferHeight = height });
    }

    static EM_BOOL emscriptenResizeCallback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData)
    {
        auto *webGpuWindow = (WebGPUWindow *)userData;
        webGpuWindow->Resize({
                .Width = uiEvent->windowInnerWidth,
                .Height = uiEvent->windowInnerHeight,
                .FramebufferWidth = uiEvent->windowInnerWidth,
                .FramebufferHeight = uiEvent->windowInnerHeight });
        return EM_TRUE;
    }

    WebGPUWindow::WebGPUWindow(const std::string &title, const uint32_t width, const uint32_t height) : m_Title(title),
            m_Width(width), m_Height(height)
    {
        if (!glfwInit())
        {
            std::cout << "GLFW init failed!" << std::endl;
            return;
        }

        glfwSetErrorCallback(glfwErrorHandler);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_Window = glfwCreateWindow((int)width, (int)height, "Figment - WebGPU", nullptr, nullptr);

        if (!m_Window)
        {
            glfwTerminate();
            std::cout << "Failed to create GLFW window!" << std::endl;
            exit(EXIT_FAILURE);
        }

        FIG_LOG_INFO("Initialized WebGPU window (%dx%d)", width, height);

        m_GfxContext = std::make_shared<WebGPUContext>();
        m_GfxContext->Init(m_Width, m_Height);

        int w, h;
        glfwGetWindowSize(m_Window, &w, &h);
        FIG_LOG_INFO("GLFW window size: %dx%d", w, h);

        int fw, fh;
        glfwGetFramebufferSize(m_Window, &fw, &fh);
        FIG_LOG_INFO("GLFW framebuffer size: %dx%d", fw, fh);
        m_FramebufferWidth = (uint32_t)fw;
        m_FramebufferHeight = (uint32_t)fh;

        emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, emscriptenResizeCallback);
        // glfwSetWindowSizeCallback(m_Window, glfwWindowSizeCallback);
        glfwSetFramebufferSizeCallback(m_Window, glfwFramebufferSizeCallback);
        glfwSetWindowUserPointer(m_Window, this);
    }

    WebGPUWindow::~WebGPUWindow()
    {
        glfwDestroyWindow(m_Window);
    }

    void WebGPUWindow::Resize(WindowResizeEventData resizeData)
    {
        m_Width = resizeData.Width;
        m_Height = resizeData.Height;
        m_FramebufferWidth = resizeData.FramebufferWidth;
        m_FramebufferHeight = resizeData.FramebufferHeight;

        glfwSetWindowSize(m_Window, (int)m_Width, (int)m_Height);

        m_GfxContext->CreateSwapChain(resizeData.Width, resizeData.Height);

        if (ResizeEventCallback != nullptr)
        {
            ResizeEventCallback(resizeData);
        }
    }

    bool WebGPUWindow::ShouldClose()
    {
        return glfwWindowShouldClose(m_Window);
    }

    uint32_t WebGPUWindow::GetWidth() const
    {
        return m_Width;
    }

    uint32_t WebGPUWindow::GetHeight() const
    {
        return m_Height;
    }

    uint32_t WebGPUWindow::GetFramebufferWidth() const
    {
        return m_FramebufferWidth;
    }

    uint32_t WebGPUWindow::GetFramebufferHeight() const
    {
        return m_FramebufferHeight;
    }

    void WebGPUWindow::SetResizeEventCallback(ResizeEventCallbackFn callback)
    {
        ResizeEventCallback = callback;
    }
}
