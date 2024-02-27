#pragma once
#include <cstdint>
#include "WebGPUContext.h"
#include "WebGPUTexture.h"
#include "glm/vec4.hpp"

namespace Figment
{
    class PixelCanvas
    {
    public:
        PixelCanvas(WebGPUContext &context, uint32_t width, uint32_t height);
        ~PixelCanvas() = default;

        void SetPixel(uint32_t x, uint32_t y, uint32_t color);
        void UpdateTexture();
        uint32_t GetWidth() const
        { return m_Width; }
        uint32_t GetHeight() const
        { return m_Height; }
    private:
        WebGPUContext &m_Context;
        uint32_t m_Width;
        uint32_t m_Height;
        WebGPUTexture m_Texture;
        uint32_t *m_Pixels;
    };

}
