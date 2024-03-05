#pragma once
#include <cstdint>
#include "WebGPUContext.h"
#include "WebGPUTexture.h"
#include "MeshRenderer.h"
#include "glm/vec4.hpp"
#include "PerspectiveCamera.h"

namespace Figment
{
    class PixelCanvas
    {
    public:
        PixelCanvas(WebGPUContext &context, uint32_t width, uint32_t height);
        ~PixelCanvas() = default;

        void OnUpdate(PerspectiveCamera &camera, float deltaTime);
        void SetPixel(uint32_t x, uint32_t y, uint32_t color);
        uint32_t GetPixel(uint32_t x, uint32_t y)
        { return m_Pixels[y * m_Width + x]; }
        void UpdateTexture();
        uint32_t GetWidth() const
        { return m_Width; }
        uint32_t GetHeight() const
        { return m_Height; }
    private:
        WebGPUContext &m_Context;
        MeshRenderer m_MeshRenderer;
        uint32_t m_Width;
        uint32_t m_Height;
        WebGPUTexture m_Texture;
        uint32_t *m_Pixels;
        Mesh *m_Mesh;
    };

}
