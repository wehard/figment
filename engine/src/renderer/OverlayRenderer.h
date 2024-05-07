#pragma once

#include "WebGPUContext.h"
#include "WebGPUShader.h"
#include "WebGPUBuffer.h"
#include "Camera.h"
#include "RenderPipeline.h"
#include "Mesh.h"

namespace Figment
{
    class OverlayRenderer
    {
    public:
        explicit OverlayRenderer(WebGPUContext &context);
        ~OverlayRenderer() = default;
        void Begin(Camera &camera);
        void End();
        void DrawGrid();
    private:
        struct CameraData
        {
            glm::mat4 ViewMatrix;
            glm::mat4 ProjectionMatrix;
        };

        struct GridData
        {
            glm::mat4 InverseViewMatrix;
            glm::mat4 InverseProjectionMatrix;
        };

        WebGPUContext &m_Context;
        RenderTarget *m_RenderTarget;
        std::unique_ptr<BindGroup> m_GridBindGroup;
        std::unique_ptr<RenderPipeline> m_GridPipeline;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WGPURenderPassEncoder m_RenderPassEncoder;

        std::unique_ptr<WebGPUShader> m_GridShader;

        std::unique_ptr<WebGPUUniformBuffer<CameraData>> m_CameraDataUniformBuffer;
        std::unique_ptr<WebGPUUniformBuffer<GridData>> m_GridDataUniformBuffer;

        Mesh *m_GridMesh;

        CameraData m_FrameCameraData;
        GridData m_FrameGridData;
    };
}
