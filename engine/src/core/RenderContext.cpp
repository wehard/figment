#include "RenderContext.h"
#include "WebGPUContext.h"

namespace Figment
{
    std::unique_ptr<RenderContext> RenderContext::Create(void *window)
    {
        return std::make_unique<WebGPUContext>();
    }
}
