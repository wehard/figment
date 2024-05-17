#pragma once

#include "RenderContext.h"

#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <functional>

namespace Figment
{
    struct WindowResizeEventData
    {
        int Width;
        int Height;
        int FramebufferWidth;
        int FramebufferHeight;
    };

    class Window
    {
    public:
        using ResizeEventCallbackFn = std::function<void(WindowResizeEventData)>;

        static std::shared_ptr<Window> Create(const std::string &title, const uint32_t width, const uint32_t height);

        Window(const std::string &title, const uint32_t width, const uint32_t height) : m_Width(width),
                m_Height(height),
                m_FramebufferWidth(width), m_FramebufferHeight(height) { }
        virtual ~Window() = default;

        bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; };
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; };
        [[nodiscard]] uint32_t GetFramebufferWidth() const { return m_FramebufferWidth; };
        [[nodiscard]] uint32_t GetFramebufferHeight() const { return m_FramebufferHeight; };

        void SetResizeEventCallback(ResizeEventCallbackFn callback) { ResizeEventCallback = callback; }
        void *GetNative() { return m_Window; }

        std::shared_ptr<RenderContext> &GetRenderContext() { return m_RenderContext; }

        ResizeEventCallbackFn ResizeEventCallback;
    protected:
        GLFWwindow *m_Window;
        std::shared_ptr<RenderContext> m_RenderContext;

        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_FramebufferWidth;
        uint32_t m_FramebufferHeight;
    };
}
