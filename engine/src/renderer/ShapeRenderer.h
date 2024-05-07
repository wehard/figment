#pragma once

#include "WebGPUContext.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "WebGPUShader.h"
#include "Camera.h"
#include "webgpu/webgpu.h"
#include "glm/glm.hpp"
#include "WebGPURenderPipeline.h"
#include "Component.h"
#include "Mesh.h"
#include "WebGPUCommand.h"
#include "RenderTarget.h"

namespace Figment
{
    struct CameraData
    {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
    };

    struct GridData
    {
        glm::mat4 ModelMatrix;
        glm::mat4 InverseViewMatrix;
        glm::mat4 InverseProjectionMatrix;
    };

    constexpr uint32_t MaxCircleCount = 1000000;
    constexpr uint32_t MaxQuadCount = 1000000;
    constexpr uint32_t MaxCircleVertexCount = MaxCircleCount * 6;
    constexpr uint32_t MaxQuadVertexCount = MaxQuadCount * 6;

    struct QuadVertex
    {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        int32_t Id;
    };

    struct CircleVertex
    {
        glm::vec3 WorldPosition;
        glm::vec3 LocalPosition;
        glm::vec4 Color;
        int32_t Id;
    };

    template<typename T>
    struct ComputeResultCopyOperation
    {
        WebGPUBuffer<T> &From;
        WebGPUBuffer<T> &To;
        uint32_t Size;
    };

    struct RendererData
    {
        std::vector<CircleVertex> CircleVertices;
        uint32_t CircleVertexCount = 0;

        std::vector<QuadVertex> QuadVertices;
        uint32_t QuadVertexCount = 0;

        void Init()
        {
            CircleVertices.resize(MaxCircleCount * 6);
            QuadVertices.resize(MaxQuadCount * 6);
        }

        void Reset()
        {
            CircleVertexCount = 0;
            QuadVertexCount = 0;
        }
    };

    class ShapeRenderer
    {
    public:
        explicit ShapeRenderer(WebGPUContext &context);
        ~ShapeRenderer();
        void InitShaders();
        WGPURenderPassEncoder Begin(Camera &camera);
        void End();
        void BeginComputePass();
        void EndComputePass();
        void Compute(FigmentComponent &figment);
        void SubmitQuad(glm::vec3 position, glm::vec4 color, int32_t id);
        void SubmitQuad(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id);
        void SubmitCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id);
        void SubmitCircle(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id);
        void Submit(Mesh &mesh, glm::mat4 transform, WebGPUShader &shader);
        void DrawFigment(FigmentComponent &figment, int32_t id);
        template<typename T>
        void DrawPoints(WebGPUVertexBuffer<T> &vertexBuffer, uint32_t vertexCount, WebGPUShader &shader)
        {
            auto pipeline = new WebGPURenderPipeline(m_Context.GetDevice(), shader, vertexBuffer.GetVertexLayout());
            pipeline->SetPrimitiveState(WGPUPrimitiveTopology_PointList, WGPUIndexFormat_Undefined,
                    WGPUFrontFace_CCW,
                    WGPUCullMode_None);
            pipeline->SetDepthStencilState(m_RenderTarget->Depth.TextureFormat, WGPUCompareFunction_Less, true);
            pipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                    m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
            auto colorTargetStates = std::vector<WGPUColorTargetState>({
                    {
                            .format = m_Context.GetTextureFormat(),
                            .blend = nullptr,
                            .writeMask = WGPUColorWriteMask_All
                    },
                    {
                            .format = m_IdTexture->GetTextureFormat(),
                            .blend = nullptr,
                            .writeMask = WGPUColorWriteMask_All
                    }
            });
            pipeline->SetColorTargetStates(colorTargetStates);
            pipeline->Build();

            WebGPUCommand::DrawVertices<T>(m_RenderPass, pipeline->Pipeline, pipeline->BindGroup,
                    vertexBuffer, vertexCount);

            delete pipeline;
        }

        void ReadPixel(int x, int y, const std::function<void(int32_t)> &callback);
        void OnResize(uint32_t width, uint32_t height);
    private:
        void DrawCircles();
        void DrawQuads();
    private:
        static constexpr glm::vec3 m_QuadVertices[] = {
                { -0.5, -0.5, 0.0 },
                { +0.5, -0.5, 0.0 },
                { -0.5, +0.5, 0.0 },
                { +0.5, -0.5, 0.0 },
                { +0.5, +0.5, 0.0 },
                { -0.5, +0.5, 0.0 }
        };

        static constexpr glm::vec3 m_FigmentQuadVertices[] = {
                { -0.5, -0.5, 0.0 },
                { -0.5, +0.5, 0.0 },
                { +0.5, +0.5, 0.0 },
                { +0.5, -0.5, 0.0 },
        };

        WebGPUContext &m_Context;
        WGPUCommandEncoder m_CommandEncoder = {};
        WGPUCommandEncoder m_ComputeCommandEncoder = {};
        WGPURenderPassEncoder m_RenderPass = {};
        WGPUComputePassEncoder m_ComputePass = {};
        WebGPUTexture *m_IdTexture;
        WebGPUVertexBuffer<CircleVertex> *m_CircleVertexBuffer;
        WebGPUVertexBuffer<QuadVertex> *m_QuadVertexBuffer;
        WebGPUBuffer<int32_t> *m_PixelBuffer;
        WebGPUUniformBuffer<CameraData> *m_CameraDataUniformBuffer;
        WebGPUUniformBuffer<GridData> *m_GridUniformBuffer;
        WebGPUShader *m_CircleShader;
        WebGPUShader *m_QuadShader;

        RenderTarget *m_RenderTarget;

        WebGPURenderPipeline *m_QuadPipeline;
        WebGPURenderPipeline *m_CirclePipeline;

        CameraData m_CameraData;
        RendererData m_RendererData;
    };
}
