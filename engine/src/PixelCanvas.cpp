#include "PixelCanvas.h"
#include "WebGPUCommand.h"

namespace Figment
{
    PixelCanvas::PixelCanvas(WebGPUContext &context, const PixelCanvasDescriptor &&descriptor) : m_Context(context),
            m_Width(descriptor.Width), m_Height(descriptor.Height),
            m_Texture(context.GetDevice(), {
                    .Format = WGPUTextureFormat_RGBA8Unorm,
                    .Width = descriptor.Width,
                    .Height = descriptor.Height,
                    .Usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_StorageBinding
                            | WGPUTextureUsage_CopyDst | WGPUTextureUsage_CopySrc,
                    .Label = "PixelCanvasTexture" })
    {
        m_PixelData = new uint32_t[descriptor.Width * descriptor.Height];

        auto aspect = (float)descriptor.Height / (float)descriptor.Width;
        m_Mesh = new Mesh(context.GetDevice(), {
                {{ -0.5, -0.5 * aspect, 0.0 }, { 0.0, 0.0 }},
                {{ 0.5, -0.5 * aspect, 0.0 }, { 1.0, 0.0 }},
                {{ 0.5, 0.5 * aspect, 0.0 }, { 1.0, 1.0 }},
                {{ -0.5, 0.5 * aspect, 0.0 }, { 0.0, 1.0 }},
        }, {
                0, 1, 2, 2, 3, 0,
        });
    }

    void PixelCanvas::SetPixel(uint32_t x, uint32_t y, const uint32_t color)
    {
        if (x < m_Width && y < m_Height)
            m_PixelData[y * m_Width + x] = color;
    }

    void PixelCanvas::Fill(uint32_t color)
    {
        for (auto i = 0; i < m_Width * m_Height; i++)
            m_PixelData[i] = color;
    }

    void PixelCanvas::UpdateTexture()
    {
        m_Texture.SetData(m_PixelData, m_Width * m_Height * sizeof(uint32_t));
    }
}
