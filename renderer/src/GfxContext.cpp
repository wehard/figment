#include "Core.h"
#include "GfxContext.h"
#include "OpenGLContext.h"
#ifdef FIGMENT_WEB
#include "WebGPUContext.h"
#endif

std::unique_ptr<GfxContext> GfxContext::Create(void *window)
{
#ifdef FIGMENT_MACOS
    return std::make_unique<OpenGLContext>(static_cast<GLFWwindow *>(window));
#elif defined(FIGMENT_WEB)
    return std::make_unique<WebGPUContext>();
#endif
}
