#include "GfxContext.h"
#include "WebGPUContext.h"

namespace Figment
{
    std::unique_ptr<GfxContext> GfxContext::Create(void *window)
    {
        return std::make_unique<WebGPUContext>();
    }
}
