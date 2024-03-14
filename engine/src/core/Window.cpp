#include "Window.h"
#include "FigmentAssert.h"
#include "WebGPUWindow.h"
#include <string>

namespace Figment
{
    std::shared_ptr<Window> Window::Create(const std::string &title, const uint32_t width, const uint32_t height)
    {
        return std::make_shared<WebGPUWindow>(title, width, height);
    }
}
