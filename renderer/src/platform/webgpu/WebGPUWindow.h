#pragma once

#include "Window.h"
#include "GfxContext.h"
#include <string>
#include <GLFW/glfw3.h>
#include <cstdint>

class WebGPUWindow : public Window
{
public:
    WebGPUWindow(const std::string &title, const uint32_t width, const uint32_t height);
    ~WebGPUWindow() override;
    void Resize(WindowResizeEventData resizeData);
    bool ShouldClose() override;
    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;
    uint32_t GetFramebufferWidth() const override;
    uint32_t GetFramebufferHeight() const override;
    void SetResizeEventCallback(ResizeEventCallbackFn callback) override;
    void *GetNative() override { return m_Window; };

private:
    GLFWwindow *m_Window;
    std::string m_Title;
    uint32_t m_Width;
    uint32_t m_Height;
    uint32_t m_FramebufferWidth;
    uint32_t m_FramebufferHeight;
    std::unique_ptr<GfxContext> m_GfxContext;
};
