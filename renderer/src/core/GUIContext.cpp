#include "GUIContext.h"
#include "Core.h"
#ifdef FIGMENT_MACOS
#include "OpenGLGUIContext.h"
#elif defined(FIGMENT_WEB)
#include "WebGPUGUIContext.h"
#endif

namespace Figment
{
    std::unique_ptr<GUIContext> GUIContext::Create()
    {
#ifdef FIGMENT_MACOS
        return std::make_unique<OpenGLGUIContext>();
#elif defined(FIGMENT_WEB)
        return std::make_unique<WebGPUGUIContext>();
#endif
    }
}
