#include "PixelRenderer.h"

namespace Figment
{
    PixelRenderer::PixelRenderer(WebGPUContext &context, uint32_t width, uint32_t height) : m_Context(context),
            m_Width(width), m_Height(height)
    {
    }
}
