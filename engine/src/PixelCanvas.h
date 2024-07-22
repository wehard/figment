#pragma once
#include <cstdint>
#include "WebGPUContext.h"
#include "WebGPUTexture.h"
#include "MeshRenderer.h"
#include "glm/vec4.hpp"
#include "PerspectiveCamera.h"

namespace Figment
{
    struct PixelCanvasDescriptor
    {
        uint32_t Width;
        uint32_t Height;
    };

    struct PixelData
    {
        uint32_t *Data;
        uint32_t Width;
        uint32_t Height;
        [[nodiscard]] uint32_t ByteSize() const { return Width * Height * sizeof(uint32_t); }
    };

    class PixelCanvas
    {
    public:
        PixelCanvas(WebGPUContext &context, const PixelCanvasDescriptor &&descriptor);
        ~PixelCanvas() = default;

        void SetPixel(uint32_t x, uint32_t y, uint32_t color);
        void Fill(uint32_t color);
        [[nodiscard]] uint32_t GetWidth() const { return m_Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        WebGPUTexture &GetTexture() { return m_Texture; }
        uint32_t *GetPixelData() { return m_PixelData; }
        Mesh &GetMesh() { return *m_Mesh; }
        void UpdateTexture();

    private:
        WebGPUContext &m_Context;
        uint32_t m_Width;
        uint32_t m_Height;
        WebGPUTexture m_Texture;
        uint32_t *m_PixelData;
        Mesh *m_Mesh;
    };

}
