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

    constexpr uint32_t MaxCircleCount = 10000;
    constexpr uint32_t MaxQuadCount = 10000;
    constexpr uint32_t MaxLineCount = 10000;
    constexpr uint32_t MaxCircleVertexCount = MaxCircleCount * 6;
    constexpr uint32_t MaxQuadVertexCount = MaxQuadCount * 6;
    constexpr uint32_t MaxLineVertexCount = MaxLineCount * 2;

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

    struct LineVertex
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

        std::vector<LineVertex> LineVertices;
        uint32_t LineVertexCount = 0;

        void Init()
        {
            CircleVertices.resize(MaxCircleVertexCount);
            QuadVertices.resize(MaxQuadVertexCount);
            LineVertices.resize(MaxLineVertexCount);
        }

        void Reset()
        {
            CircleVertexCount = 0;
            QuadVertexCount = 0;
            LineVertexCount = 0;
        }
    };

    class ShapeRenderer
    {
    public:
        explicit ShapeRenderer(WebGPUContext &context);
        ~ShapeRenderer();
        void InitShaders();
        void Begin(Camera &camera);
        void End();
        void SubmitQuad(glm::vec3 position, glm::vec4 color, int32_t id);
        void SubmitQuad(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id);
        void SubmitCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id);
        void SubmitCircle(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id);
        void SubmitLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, int32_t id);
        void ReadPixel(int x, int y, const std::function<void(int32_t)> &callback);
        void OnResize(uint32_t width, uint32_t height);
    private:
        void DrawCircles();
        void DrawQuads();
        void DrawLines();
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
        WGPURenderPassEncoder m_RenderPass = {};
        WebGPUTexture *m_IdTexture;
        WebGPUVertexBuffer<CircleVertex> *m_CircleVertexBuffer;
        WebGPUVertexBuffer<QuadVertex> *m_QuadVertexBuffer;
        WebGPUVertexBuffer<LineVertex> *m_LineVertexBuffer;
        WebGPUBuffer<int32_t> *m_PixelBuffer;
        WebGPUUniformBuffer<CameraData> *m_CameraDataUniformBuffer;
        WebGPUUniformBuffer<GridData> *m_GridUniformBuffer;
        WebGPUShader *m_CircleShader;
        WebGPUShader *m_QuadShader;
        WebGPUShader *m_LineShader;

        RenderTarget *m_RenderTarget;

        WebGPURenderPipeline *m_QuadPipeline;
        WebGPURenderPipeline *m_CirclePipeline;
        WebGPURenderPipeline *m_LinePipeline;

        CameraData m_CameraData;
        RendererData m_RendererData;
    };
}
