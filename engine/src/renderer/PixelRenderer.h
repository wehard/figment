#pragma once

#include "WebGPUContext.h"

namespace Figment
{
    class PixelRenderer
    {
    public:
        PixelRenderer(WebGPUContext &context, uint32_t width, uint32_t height);
        virtual ~PixelRenderer() = default;
    private:
        WebGPUContext &m_Context;
        uint32_t m_Width;
        uint32_t m_Height;
    };

}
