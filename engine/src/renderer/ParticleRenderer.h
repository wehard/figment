#pragma once

#include "webgpu/webgpu.h"
#include "RenderPass.h"
#include "ComputePass.h"
#include "WebGPURenderPipeline.h"
#include "WebGPUCommand.h"
#include "Camera.h"

namespace Figment
{
    class ParticleRenderer
    {
    public:
        struct CameraData
        {
            glm::mat4 ViewMatrix;
            glm::mat4 ProjectionMatrix;
        };

        ParticleRenderer(WebGPUContext &context);
        ~ParticleRenderer();
        void CreateDefaultPipeline(WebGPUShader &shader, WGPUVertexBufferLayout &vertexBufferLayout);
        void BeginFrame(Camera &camera);
        void EndFrame();

        template<typename T>
        void DrawPoints(WebGPUVertexBuffer<T> &vertexBuffer, WebGPUShader &shader)
        {
            if (m_Pipeline == nullptr)
            {
                auto layout = vertexBuffer.GetVertexLayout();
                CreateDefaultPipeline(shader, layout);
            }

            WebGPUCommand::DrawVertices<T>(m_RenderPass, m_Pipeline, m_BindGroup,
                    vertexBuffer, vertexBuffer.Count());
        }
        void OnResize(uint32_t width, uint32_t height);
    private:
        WebGPUContext &m_Context;
        WGPUDevice m_Device;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WGPUTextureView m_RenderTarget = nullptr;
        WGPUTextureFormat m_RenderTargetTextureFormat;
        WebGPUTexture *m_DepthTexture;
        WebGPUUniformBuffer<CameraData> *m_CameraDataUniformBuffer;

        WGPURenderPassEncoder m_RenderPass = nullptr;
        WGPURenderPipeline m_Pipeline = nullptr;
        WGPUBindGroup m_BindGroup = nullptr;
    };
}
