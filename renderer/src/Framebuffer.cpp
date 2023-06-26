#include "Framebuffer.h"
#include "OpenGLFramebuffer.h"

std::unique_ptr<Framebuffer> Framebuffer::Create(uint32_t width, uint32_t height)
{
	return std::make_unique<OpenGLFramebuffer>(width, height);
}
