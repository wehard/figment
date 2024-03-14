#include "GUIContext.h"
#include "WebGPUGUIContext.h"

namespace Figment
{
    std::unique_ptr<GUIContext> GUIContext::Create()
    {
        return std::make_unique<WebGPUGUIContext>();
    }
}
