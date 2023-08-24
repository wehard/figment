#include "Framebuffer.h"
#include "Core.h"

#ifdef FIGMENT_MACOS
#include "OpenGLFramebuffer.h"
#endif

std::unique_ptr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height)
{

#ifdef FIGMENT_MACOS
	return std::make_unique<OpenGLFramebuffer>(width, height);
#elif defined(FIGMENT_WEB)
    return nullptr;
#elif
    FIGMENT_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}
