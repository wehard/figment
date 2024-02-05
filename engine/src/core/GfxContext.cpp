#include "Core.h"
#include "GfxContext.h"
#ifdef FIGMENT_WEB
#include "WebGPUContext.h"
#endif

namespace Figment
{
    std::unique_ptr<GfxContext> GfxContext::Create(void *window)
    {
        return CreateUniquePtr<WebGPUContext>();
    }
}
