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

namespace Figment
{
    struct CameraData
    {
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
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

    struct RendererStats
    {
        uint32_t DrawCalls = 0;
        uint32_t VertexCount = 0;
        uint32_t CircleCount = 0;
        uint32_t QuadCount = 0;

        void Reset()
        {
            DrawCalls = 0;
            VertexCount = 0;
            CircleCount = 0;
            QuadCount = 0;
        }
    };

    class WebGPURenderer
    {
    public:
        explicit WebGPURenderer(WebGPUContext &context);
        ~WebGPURenderer();
        void InitShaders();
        WGPURenderPassEncoder Begin(Figment::Camera &camera);
        void End();
        void BeginComputePass();
        void EndComputePass();
        void Compute(Figment::FigmentComponent &figment);
        void DrawQuad(glm::vec3 position, glm::vec4 color, int32_t id);
        void DrawQuad(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id);
        void DrawCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id);
        void DrawCircle(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id);
        void ReadPixel(int x, int y, const std::function<void(int32_t)> &callback);
        void OnResize(uint32_t width, uint32_t height);

        static RendererStats GetStats()
        { return s_Stats; }
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
        WebGPUShader *m_CircleShader;
        WebGPUShader *m_QuadShader;
        WebGPUTexture *m_DepthTexture;

        WebGPURenderPipeline *m_QuadPipeline;
        WebGPURenderPipeline *m_CirclePipeline;

        RendererData m_RendererData;
        static RendererStats s_Stats;
    };
}
