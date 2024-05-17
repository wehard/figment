#pragma once

#include "Window.h"
#include "RenderContext.h"

namespace Figment
{
    class VulkanWindow : public Window
    {
    public:
        VulkanWindow(const std::string &title, uint32_t width, uint32_t height);
    private:
    };
}
