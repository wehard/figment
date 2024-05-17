#pragma once

#include "Window.h"

namespace Figment
{
    class VulkanWindow : public Window
    {
    public:
        VulkanWindow(const std::string &title, uint32_t width, uint32_t height) : Window(title, width, height)
        {
        }

        void SetResizeEventCallback(ResizeEventCallbackFn callback) override { };
        bool ShouldClose() override { return false; }

        void *GetNative() override { return nullptr; }
    };
}
