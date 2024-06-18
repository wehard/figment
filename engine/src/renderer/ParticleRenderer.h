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
        ParticleRenderer() = delete;
        explicit ParticleRenderer(WebGPUContext &context);
        ~ParticleRenderer();

        void BeginFrame(Camera &camera);
        void EndFrame();
        void OnResize(uint32_t width, uint32_t height);

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
                                .shaderLocation = 0,
                        }
                }, sizeof(glm::vec3), WGPUVertexStepMode_Vertex);
                m_QuadVertexBuffer->SetData(m_Vertices, sizeof(m_Vertices));

                m_QuadIndexBuffer = new WebGPUIndexBuffer<uint32_t>(m_Context.GetDevice(),
                        "ParticleRendererQuadIndexBuffer", sizeof(m_Indices));
                m_QuadIndexBuffer->SetData(m_Indices, sizeof(m_Indices));

                BindGroup bindGroup(m_Context.GetDevice(), WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
                bindGroup.Bind(*m_CameraDataUniformBuffer);
                bindGroup.Bind(*m_ParticlesDataUniformBuffer);

                RenderPipeline pipeline = RenderPipeline(m_Context.GetDevice(), {
                        .Shader = shader,
                        .BindGroup = bindGroup,
                        .VertexBufferLayouts = { m_QuadVertexBuffer->GetVertexLayout(),
                                                 particlePositions.GetVertexLayout() },
                        .ColorTargetStates = {
                                { m_Context.GetDefaultRenderTarget()->Color.TextureFormat, WGPUColorWriteMask_All }},
                        .DepthStencilStates = {{ m_Context.GetDefaultRenderTarget()->Depth.TextureFormat, true,
                                                 WGPUCompareFunction_Less }},
                        .PrimitiveState = {
                                .topology = WGPUPrimitiveTopology_TriangleList,
                                .stripIndexFormat = WGPUIndexFormat_Undefined,
                                .frontFace = WGPUFrontFace_CCW,
                                .cullMode = WGPUCullMode_None
                        },
                        .MultisampleState = { .count = 1, .mask = 0xFFFFFFFF, .alphaToCoverageEnabled = false }
                });

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

            RenderStats::ParticleCount += particlePositions.Count();
            RenderStats::VertexCount += particlePositions.Count() * m_QuadVertexBuffer->Count();
            RenderStats::DrawCalls++;
        }

    private:
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

        glm::vec3 m_Vertices[4] = {
                { -0.5, -0.5, 0.0 },
                { -0.5, +0.5, 0.0 },
                { +0.5, +0.5, 0.0 },
                { +0.5, -0.5, 0.0 },
        };

        uint32_t m_Indices[6] = { 0, 1, 2, 0, 2, 3 };

    private:
        WebGPUContext &m_Context;
        WGPUDevice m_Device = nullptr;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WGPURenderPassEncoder m_RenderPass = nullptr;
        WGPURenderPipeline m_Pipeline = nullptr;
        WGPUBindGroup m_BindGroup = nullptr;

        WebGPUVertexBuffer<glm::vec3> *m_QuadVertexBuffer = nullptr;
        WebGPUIndexBuffer<uint32_t> *m_QuadIndexBuffer = nullptr;
        WebGPUUniformBuffer<CameraData> *m_CameraDataUniformBuffer;
        WebGPUUniformBuffer<ParticlesData> *m_ParticlesDataUniformBuffer;
    };
}
