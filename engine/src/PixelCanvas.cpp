#include "PixelCanvas.h"
#include "WebGPUCommand.h"

namespace Figment
{
    PixelCanvas::PixelCanvas(WebGPUContext &context, uint32_t width, uint32_t height) : m_Context(context),
            m_Width(width), m_Height(height),
            m_Texture(context.GetDevice(), WGPUTextureFormat_RGBA8Unorm, width, height,
                    WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, "PixelCanvasTexture"),
            m_MeshRenderer(context)
    {
        m_Pixels = new uint32_t[width * height];

        auto aspectRatio = (float)height / (float)width;
        std::vector<Mesh::Vertex> vertices = {
                {{ -0.5, -0.5 * aspectRatio, 0.0 }},
                {{ 0.5, -0.5 * aspectRatio, 0.0 }},
                {{ 0.5, 0.5 * aspectRatio, 0.0 }},
                {{ -0.5, 0.5 * aspectRatio, 0.0 }},
        };

        std::vector<uint32_t> indices = {
                0, 1, 2, 2, 3, 0,
                1, 5, 6, 6, 2, 1,
        };

        m_Mesh = new Figment::Mesh(context.GetDevice(), vertices, indices);
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

    void PixelCanvas::OnUpdate(PerspectiveCamera &camera, float deltaTime)
    {
        m_MeshRenderer.BeginFrame(camera);
        m_MeshRenderer.DrawTextured(*m_Mesh, glm::mat4(1.0f), m_Texture);
        m_MeshRenderer.EndFrame();
    }
}
