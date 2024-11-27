#pragma once

#include "RenderContext.h"

// #include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <string>

class GLFWwindow;

namespace figment
{

class Window
{
public:
    struct ResizeEventData
    {
        int Width;
        int Height;
        int FramebufferWidth;
        int FramebufferHeight;
    };
    using ResizeEventCallbackFn = std::function<void(ResizeEventData)>;

    static std::shared_ptr<Window> Create(const std::string& title, uint32_t width,
                                          uint32_t height);

    virtual ~Window() = default;

    bool ShouldClose() { return false; }
    [[nodiscard]] uint32_t GetWidth() const { return m_Width; };
    [[nodiscard]] uint32_t GetHeight() const { return m_Height; };
    [[nodiscard]] uint32_t GetFramebufferWidth() const { return m_FramebufferWidth; };
    [[nodiscard]] uint32_t GetFramebufferHeight() const { return m_FramebufferHeight; };

    void SetResizeEventCallback(ResizeEventCallbackFn callback) { ResizeEventCallback = callback; }
    [[nodiscard]] GLFWwindow* GetNative() const { return window; }

    std::shared_ptr<RenderContext> GetContext() { return renderContext; }

    template<class T>
    std::shared_ptr<T> GetContext() const
    {
        return std::static_pointer_cast<T>(renderContext);
    }

    ResizeEventCallbackFn ResizeEventCallback;
    void OnResize(ResizeEventData resizeData)
    {
        m_Width             = resizeData.Width;
        m_Height            = resizeData.Height;
        m_FramebufferWidth  = resizeData.FramebufferWidth;
        m_FramebufferHeight = resizeData.FramebufferHeight;

        renderContext->onResize(resizeData.Width, resizeData.Height);

        if (ResizeEventCallback != nullptr)
        {
            ResizeEventCallback(resizeData);
        }
    }

protected:
    Window(const std::string& title, const uint32_t width, const uint32_t height):
        m_Width(width), m_Height(height), m_FramebufferWidth(width), m_FramebufferHeight(height)
    {
    }

    GLFWwindow* window                           = nullptr;
    std::shared_ptr<RenderContext> renderContext = nullptr;

    uint32_t m_Width                             = 0;
    uint32_t m_Height                            = 0;
    uint32_t m_FramebufferWidth                  = 0;
    uint32_t m_FramebufferHeight                 = 0;
};
} // namespace figment
