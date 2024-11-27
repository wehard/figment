#pragma once

#include "RenderContext.h"
#include "Window.h"

namespace figment
{
class VulkanWindow: public Window
{
public:
    VulkanWindow(const std::string& title, uint32_t width, uint32_t height);

private:
};
} // namespace figment
