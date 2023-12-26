#include "Core.h"
#include "Framebuffer.h"

#ifdef FIGMENT_MACOS
#include "OpenGLFramebuffer.h"
#elif defined(FIGMENT_WEB)
#include "WebGPUFramebuffer.h"
#endif

namespace Figment
{
    std::unique_ptr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height)
    {

#ifdef FIGMENT_MACOS
        return std::make_unique<OpenGLFramebuffer>(width, height);
#elif defined(FIGMENT_WEB)
        return std::make_unique<WebGPUFramebuffer>(width, height);
#elif
        FIGMENT_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }
}
