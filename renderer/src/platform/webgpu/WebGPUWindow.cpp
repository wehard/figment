#include "WebGPUWindow.h"
#include <iostream>

static void glfwErrorHandler(int error, const char *message)
{
    std::cout << "Glfw error: " << error << ": " << message << std::endl;
}

static void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    auto *openGlWindow = (WebGPUWindow*)glfwGetWindowUserPointer(window);
    openGlWindow->Resize({
        .Width = width,
        .Height = height,
        .FramebufferWidth = static_cast<int>(openGlWindow->GetFramebufferWidth()),
        .FramebufferHeight = static_cast<int>(openGlWindow->GetFramebufferHeight())});
}

static void glfwFramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    auto *openGlWindow = (WebGPUWindow *) glfwGetWindowUserPointer(window);
    openGlWindow->Resize({
        .Width = static_cast<int>(openGlWindow->GetWidth()),
        .Height = static_cast<int>(openGlWindow->GetHeight()),
        .FramebufferWidth = width,
        .FramebufferHeight = height});
}

WebGPUWindow::WebGPUWindow(const std::string &title, const uint32_t width, const uint32_t height) : m_Title(title), m_Width(width), m_Height(height)
{
    if (!glfwInit())
    {
        std::cout << "GLFW init failed!" << std::endl;
        return;
    }

    glfwSetErrorCallback(glfwErrorHandler);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//    glfwWindowHint(GLFW_SAMPLES, 4);

    m_Window = glfwCreateWindow(width, height, "Figment - WebGPU", nullptr, nullptr);

    if (!m_Window)
    {
        glfwTerminate();
        std::cout << "Failed to create GLFW window!" << std::endl;
        exit(EXIT_FAILURE);
    }

    printf("Initialized WebGPU window (%dx%d)\n", width, height);

    m_GfxContext = std::make_shared<WebGPUContext>();
    m_GfxContext->Init();

    glfwShowWindow(m_Window);

//    int w, h;
//    glfwGetWindowSize(m_Window, &w, &h);
//    std::cout << "GLFW window created: " << w << " x " << h << std::endl;
//
//    int fw, fh;
//    glfwGetFramebufferSize(m_Window, &fw, &fh);
//    std::cout << "GLFW framebuffer size: " << fw << " x " << fh << std::endl;
//    m_FramebufferWidth = (uint32_t)fw;
//    m_FramebufferHeight = (uint32_t)fh;
//
//    glfwSetWindowSizeCallback(m_Window, glfwWindowSizeCallback);
//    glfwSetFramebufferSizeCallback(m_Window, glfwFramebufferSizeCallback);
//
//    glfwSetWindowUserPointer(m_Window, this);
}

WebGPUWindow::~WebGPUWindow()
{
    glfwDestroyWindow(m_Window);
}

void WebGPUWindow::Resize(WindowResizeEventData resizeData)
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

void WebGPUWindow::SetResizeEventCallback(ResizeEventCallbackFn callback) {
    ResizeEventCallback = callback;
}
