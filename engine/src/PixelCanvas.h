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
        bool useCompute;
    };

    class PixelCanvas
    {
    public:
        PixelCanvas(WebGPUContext &context, PixelCanvasDescriptor *descriptor);
        ~PixelCanvas() = default;

        void OnUpdate(PerspectiveCamera &camera, glm::mat4 transform);
        void SetPixel(uint32_t x, uint32_t y, uint32_t color);
        void Fill(uint32_t color);
        uint32_t GetPixel(uint32_t x, uint32_t y) { return m_Pixels[y * m_Width + x]; }
        void SwapPixels(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
        void UpdateTexture();
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }
        WebGPUTexture &GetTexture() { return m_Texture; }
        WebGPUTexture &GetComputeTexture() { return *m_ComputeTexture; }
        uint32_t *GetPixelData() { return m_Pixels; }

    private:
        WebGPUContext &m_Context;
        MeshRenderer m_MeshRenderer;
        uint32_t m_Width;
        uint32_t m_Height;
        WebGPUTexture m_Texture;
        uint32_t *m_Pixels;
        Mesh *m_Mesh;

        bool m_UseCompute = false;
        std::unique_ptr<WebGPUTexture> m_ComputeTexture = nullptr;
    };

}
