#include "GfxContext.h"
#include "OpenGLContext.h"

std::unique_ptr<GfxContext> GfxContext::Create(void *window)
{
    return std::make_unique<OpenGLContext>(static_cast<GLFWwindow *>(window));
}
