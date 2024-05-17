#include "Window.h"
#include "FigmentAssert.h"
#include <string>

#ifdef __EMSCRIPTEN__
#include "WebGPUWindow.h"
#endif

namespace Figment
{
    std::shared_ptr<Window> Window::Create(const std::string &title, const uint32_t width, const uint32_t height)
    {
#ifdef __EMSCRIPTEN__
        return std::make_shared<WebGPUWindow>(title, width, height);
#endif
        FIGMENT_ASSERT(false, "Window creation not implemented for this platform");
        return nullptr;
    }
}
