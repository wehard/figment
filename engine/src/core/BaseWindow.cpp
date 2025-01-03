#include "BaseWindow.h"
#include "FigmentAssert.h"
#include <string>

#if defined(WEBGPU_BACKEND)
#include "WebGPUWindow.h"
#elif defined(VULKAN_BACKEND)
#include "window.h"
#endif

namespace figment
{
std::shared_ptr<Window> Window::Create(const std::string& title, const uint32_t width,
                                       const uint32_t height)
{
#if defined(WEBGPU_BACKEND)
    return std::make_shared<WebGPUWindow>(title, width, height);
#elif defined(VULKAN_BACKEND)
    return std::make_shared<vulkan::Window>(title, width, height);
#endif
    FIGMENT_ASSERT(false, "Window creation not implemented for this platform");
    return nullptr;
}
} // namespace figment
