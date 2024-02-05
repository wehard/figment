#include "Core.h"
#include "Window.h"
#include "FigmentAssert.h"
#ifdef __EMSCRIPTEN__
#include "WebGPUWindow.h"
#endif
#include <string>

namespace Figment
{
    std::shared_ptr<Window> Window::Create(const std::string &title, const uint32_t width, const uint32_t height)
    {
#ifdef FIGMENT_WEB
        return std::make_shared<WebGPUWindow>(title, width, height);
#elif
        FIGMENT_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }
}
