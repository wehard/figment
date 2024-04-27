#pragma once

#include "webgpu/webgpu.h"
#include "RenderPass.h"
#include "ComputePass.h"
#include "WebGPURenderPipeline.h"
#include "WebGPUCommand.h"
#include "Camera.h"
#include "RenderStats.h"
#include "RenderPipeline.h"

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

        struct ParticlesData
        {
            glm::mat4 ModelMatrix;
            float ParticleSize;
            uint32_t _Padding[3];
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
            RenderStats::VertexCount += vertexBuffer.Count();
            RenderStats::DrawCalls++;
        }

        template<typename T>
        void DrawQuads(WebGPUVertexBuffer<T> &particlePositions, glm::mat4 transform, float particleSize,
                WebGPUShader &shader)
        {
            if (m_Pipeline == nullptr)
            {
                m_QuadVertexBuffer = new WebGPUVertexBuffer<glm::vec3>(m_Context.GetDevice(),
                        "ParticleRendererQuadVertexBuffer", sizeof(m_Vertices));
                m_QuadVertexBuffer->SetVertexLayout({
                        {
                                .format = WGPUVertexFormat_Float32x3,
                                .offset = 0,
                                .shaderLocation = 1,
                        }
                }, sizeof(glm::vec3), WGPUVertexStepMode_Vertex);
                m_QuadVertexBuffer->SetData(m_Vertices, sizeof(m_Vertices));

                m_QuadIndexBuffer = new WebGPUIndexBuffer<uint32_t>(m_Context.GetDevice(),
                        "ParticleRendererQuadIndexBuffer", sizeof(m_Indices));
                m_QuadIndexBuffer->SetData(m_Indices, sizeof(m_Indices));

                BindGroup bindGroup(m_Context.GetDevice(), WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
                bindGroup.Bind(*m_CameraDataUniformBuffer);
                bindGroup.Bind(*m_ParticlesDataUniformBuffer);

                RenderPipeline pipeline(m_Context.GetDevice(), shader, bindGroup,
                        { m_QuadVertexBuffer->GetVertexLayout(), particlePositions.GetVertexLayout() });
                pipeline.SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                        WGPUFrontFace_CCW,
                        WGPUCullMode_None);
                pipeline.SetDepthStencilState(m_DepthTexture->GetTextureFormat(), true, WGPUCompareFunction_Less);
                pipeline.AddColorTarget(m_RenderTargetTextureFormat, WGPUColorWriteMask_All);

                m_Pipeline = pipeline.Get();
                m_BindGroup = bindGroup.Get();

            }

            ParticlesData data = {
                    .ModelMatrix = transform,
                    .ParticleSize = particleSize
            };

            m_ParticlesDataUniformBuffer->SetData(&data, sizeof(data));

            wgpuRenderPassEncoderSetIndexBuffer(m_RenderPass, m_QuadIndexBuffer->GetBuffer(),
                    WGPUIndexFormat_Uint32, 0,
                    m_QuadIndexBuffer->GetSize());
            wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_QuadVertexBuffer->GetBuffer(), 0,
                    m_QuadVertexBuffer->GetSize());
            wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 1, particlePositions.GetBuffer(), 0,
                    particlePositions.GetSize());
            wgpuRenderPassEncoderSetPipeline(m_RenderPass, m_Pipeline);
            wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, m_BindGroup, 0, nullptr);
            wgpuRenderPassEncoderDrawIndexed(m_RenderPass, m_QuadIndexBuffer->Count(), particlePositions.Count(), 0, 0,
                    0);

            RenderStats::VertexCount += particlePositions.Count() * m_QuadVertexBuffer->Count();
            RenderStats::DrawCalls++;
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
        WebGPUUniformBuffer<ParticlesData> *m_ParticlesDataUniformBuffer;

        WGPURenderPassEncoder m_RenderPass = nullptr;
        WGPURenderPipeline m_Pipeline = nullptr;
        WGPUBindGroup m_BindGroup = nullptr;

        glm::vec3 m_Vertices[6] = {
                { -0.5, 0.0, 0.5 },
                { -0.5, 0.0, -0.5 },
                { +0.5, 0.0, -0.5 },
                { +0.5, 0.0, 0.5 },
                { 0.0, 0.5, 0.0 },
                { 0.0, -0.5, 0.0 },
        };

        uint32_t m_Indices[24] = { 0, 4, 1, 1, 4, 2, 2, 4, 3, 3, 4, 0,
                                   0, 1, 5, 1, 2, 5, 2, 3, 5, 3, 0, 5 };

        WebGPUVertexBuffer<glm::vec3> *m_QuadVertexBuffer;
        WebGPUIndexBuffer<uint32_t> *m_QuadIndexBuffer;
    };
}
