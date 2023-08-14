#pragma once

#include <memory>
#include <string>

class Window
{
public:
    using ResizeEventCallbackFn = std::function<void(uint32_t, uint32_t)>;
    virtual ~Window() = default;

    virtual bool ShouldClose() = 0;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetFramebufferWidth() const = 0;
    virtual uint32_t GetFramebufferHeight() const = 0;
    virtual void SetResizeEventCallback(ResizeEventCallbackFn callback) = 0;

    virtual void *GetNative() = 0;

    static std::shared_ptr<Window> Create(const std::string &title, const uint32_t width, const uint32_t height);
    ResizeEventCallbackFn ResizeEventCallback;

};
