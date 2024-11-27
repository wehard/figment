#pragma once

#include "RenderContext.h"
#include "Window.h"

namespace figment::vulkan
{
class Window: public figment::Window
{
public:
    Window(const std::string& title, uint32_t width, uint32_t height);

private:
};
} // namespace figment::vulkan
