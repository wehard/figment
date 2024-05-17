#pragma once

#include "Window.h"

namespace Figment
{
    class VulkanWindow : public Window
    {
    public:
        VulkanWindow(const std::string &title, uint32_t width, uint32_t height) : m_Width(width), m_Height(height)
        {
        }

        [[nodiscard]] uint32_t GetWidth() const override { return 0; }
        [[nodiscard]] uint32_t GetHeight() const override { return 0; }
        [[nodiscard]] uint32_t GetFramebufferWidth() const override { return 0; }
        [[nodiscard]] uint32_t GetFramebufferHeight() const override { return 0; }
        void SetResizeEventCallback(ResizeEventCallbackFn callback) override { };
        bool ShouldClose() override { return false; }

        void *GetNative() override { return nullptr; }
    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };
}
