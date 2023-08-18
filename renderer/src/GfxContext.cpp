#include "GfxContext.h"
#include "OpenGLContext.h"
#ifdef __EMSCRIPTEN
#include "WebGPUContext.h"
#endif

std::unique_ptr<GfxContext> GfxContext::Create(void *window)
{
#ifdef __EMSCRIPTEN__
    return std::make_unique<WebGPUContext>();
#endif
    return std::make_unique<OpenGLContext>(static_cast<GLFWwindow *>(window));
}
