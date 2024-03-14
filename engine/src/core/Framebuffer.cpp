#include "Framebuffer.h"

#include "WebGPUFramebuffer.h"

namespace Figment
{
    std::unique_ptr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height)
    {
        return std::make_unique<WebGPUFramebuffer>(width, height);
    }
}
