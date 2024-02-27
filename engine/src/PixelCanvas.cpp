#include "PixelCanvas.h"
#include "WebGPUCommand.h"

namespace Figment
{
    PixelCanvas::PixelCanvas(WebGPUContext &context, uint32_t width, uint32_t height) : m_Context(context),
            m_Width(width), m_Height(height),
            m_Texture(context.GetDevice(), WGPUTextureFormat_RGBA8Unorm, width, height,
                    WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, "PixelCanvasTexture")
    {
        m_Pixels = new uint32_t[width * height];
    }

    void PixelCanvas::SetPixel(uint32_t x, uint32_t y, const uint32_t color)
    {
        if (x < m_Width && y < m_Height)
            m_Pixels[y * m_Width + x] = color;
    }

    void PixelCanvas::UpdateTexture()
    {
        m_Texture.SetData(m_Pixels, m_Width * m_Height * sizeof(uint32_t));
    }
}
