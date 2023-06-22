#pragma once

#include "Window.h"
#include "GfxContext.h"
#include <string>
#include <GLFW/glfw3.h>
#include <cstdint>

class OpenGLWindow : public Window
{
public:
    OpenGLWindow(const std::string &title, const uint32_t width, const uint32_t height);
    ~OpenGLWindow() override;
    void Resize(uint32_t width, uint32_t height);
    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;
    uint32_t GetFramebufferWidth() const override;
    uint32_t GetFramebufferHeight() const override;
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
