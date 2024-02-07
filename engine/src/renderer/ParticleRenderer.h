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
        void BeginFrame(Camera &camera);
        void EndFrame();

        template<typename T>
        void DrawPoints(WebGPUVertexBuffer<T> &vertexBuffer, WebGPUShader &shader)
        {
            auto pipeline = new WebGPURenderPipeline(m_Context.GetDevice(), shader, vertexBuffer.GetVertexLayout());
            pipeline->SetPrimitiveState(WGPUPrimitiveTopology_PointList, WGPUIndexFormat_Undefined,
                    WGPUFrontFace_CCW,
                    WGPUCullMode_None);
            pipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);
            pipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                    m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
            auto colorTargetStates = std::vector<WGPUColorTargetState>({
                    {
                            .format = m_RenderTargetTextureFormat,
                            .blend = nullptr,
                            .writeMask = WGPUColorWriteMask_All
                    }
            });
            pipeline->SetColorTargetStates(colorTargetStates);
            pipeline->Build();

            WebGPUCommand::DrawVertices<T>(m_RenderPass, pipeline->GetPipeline(), pipeline->GetBindGroup(),
                    vertexBuffer, vertexBuffer.Count());

            delete pipeline;
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
    };
}
