#include "PixelCanvas.h"
#include "WebGPUCommand.h"

namespace Figment
{
    PixelCanvas::PixelCanvas(WebGPUContext &context, PixelCanvasDescriptor *descriptor) : m_Context(context),
            m_Width(descriptor->Width), m_Height(descriptor->Height),
            m_Texture(context.GetDevice(), WGPUTextureFormat_RGBA8Unorm, descriptor->Width, descriptor->Height,
                    WGPUTextureUsage_TextureBinding | WGPUTextureUsage_StorageBinding | WGPUTextureUsage_CopyDst
                            | WGPUTextureUsage_CopySrc,
                    "PixelCanvasTexture"),
            m_MeshRenderer(context), m_UseCompute(descriptor->useCompute)
    {
        m_Pixels = new uint32_t[descriptor->Width * descriptor->Height];

        auto aspectRatio = (float)descriptor->Height / (float)descriptor->Width;
        std::vector<Mesh::Vertex> vertices = {
                {{ -0.5, -0.5 * aspectRatio, 0.0 }, { 0.0, 0.0 }},
                {{ 0.5, -0.5 * aspectRatio, 0.0 }, { 1.0, 0.0 }},
                {{ 0.5, 0.5 * aspectRatio, 0.0 }, { 1.0, 1.0 }},
                {{ -0.5, 0.5 * aspectRatio, 0.0 }, { 0.0, 1.0 }},
        };

        std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0,
        };

        m_Mesh = new Figment::Mesh(context.GetDevice(), vertices, indices);

        if (m_UseCompute)
        {
            m_ComputeTexture = CreateUniquePtr<WebGPUTexture>(context.GetDevice(), WGPUTextureFormat_RGBA8Unorm,
                    descriptor->Width, descriptor->Height,
                    WGPUTextureUsage_TextureBinding | WGPUTextureUsage_StorageBinding | WGPUTextureUsage_CopyDst
                            | WGPUTextureUsage_CopySrc,
                    "PixelCanvasComputeTexture");
        }
    }

    void PixelCanvas::SetPixel(uint32_t x, uint32_t y, const uint32_t color)
    {
        if (x < m_Width && y < m_Height)
            m_Pixels[y * m_Width + x] = color;
    }

    void PixelCanvas::Fill(uint32_t color)
    {
        for (auto i = 0; i < m_Width * m_Height; i++)
            m_Pixels[i] = color;
    }

    void PixelCanvas::SwapPixels(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
    {
        if (x1 >= m_Width || y1 >= m_Height || x2 >= m_Width || y2 >= m_Height)
            return;
        auto temp = m_Pixels[y1 * m_Width + x1];
        m_Pixels[y1 * m_Width + x1] = m_Pixels[y2 * m_Width + x2];
        m_Pixels[y2 * m_Width + x2] = temp;
    }

    void PixelCanvas::UpdateTexture()
    {
        // if (m_UseCompute)
        // {
        //     m_ComputeTexture->SetData(m_Pixels, m_Width * m_Height * sizeof(uint32_t));
        //     return;
        // }
        m_Texture.SetData(m_Pixels, m_Width * m_Height * sizeof(uint32_t));
    }

    void PixelCanvas::OnUpdate(PerspectiveCamera &camera, float deltaTime)
    {
        m_MeshRenderer.BeginFrame(camera);
        m_MeshRenderer.DrawTextured(*m_Mesh, glm::mat4(1.0f), m_Texture);
        m_MeshRenderer.EndFrame();
    }
}
