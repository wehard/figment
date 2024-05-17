#pragma once

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
        Window(const std::string &title, const uint32_t width, const uint32_t height) : m_Width(width),
                m_Height(height),
                m_FramebufferWidth(width), m_FramebufferHeight(height) { }
        virtual ~Window() = default;

        virtual bool ShouldClose() = 0;
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; };
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; };
        [[nodiscard]] uint32_t GetFramebufferWidth() const { return m_FramebufferWidth; };
        [[nodiscard]] uint32_t GetFramebufferHeight() const { return m_FramebufferHeight; };
        virtual void SetResizeEventCallback(ResizeEventCallbackFn callback) = 0;

        virtual void *GetNative() = 0;

        static std::shared_ptr<Window> Create(const std::string &title, const uint32_t width, const uint32_t height);
        ResizeEventCallbackFn ResizeEventCallback;
    protected:
        uint32_t m_Width;
        uint32_t m_Height;
        uint32_t m_FramebufferWidth;
        uint32_t m_FramebufferHeight;
    };
}
