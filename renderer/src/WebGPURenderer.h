#pragma once

#include "WebGPUContext.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "WebGPUShader.h"
#include "Camera.h"
#include "webgpu/webgpu.h"
#include "glm/glm.hpp"

struct UniformData
{
    glm::mat4 ModelMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::vec4 Color;
    uint32_t Id;
    uint32_t _padding[3];
};

struct CameraData
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

constexpr uint32_t MaxCircleCount = 30000;
constexpr uint32_t MaxQuadCount = 30000;
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
    WGPURenderPassEncoder Begin(Camera &camera);
    void End();
    void DrawQuad(glm::vec3 position, glm::vec4 color, int32_t id);
    void DrawCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id);
    void ReadPixel(int x, int y, std::function<void(int32_t)> callback);
    void OnResize(uint32_t width, uint32_t height);
    WebGPUShader *GetShader()
    { return m_Shader; }
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
    WGPURenderPassEncoder m_RenderPass = {};
    CameraData m_CameraData = {};
    WebGPUTexture *m_IdTexture;
    WebGPUBuffer<CircleVertex> *m_CircleVertexBuffer;
    WebGPUBuffer<QuadVertex> *m_QuadVertexBuffer;
    WebGPUBuffer<std::byte> *m_PixelBuffer;
    WebGPUShader *m_Shader;
    WebGPUShader *m_CircleShader;
    WebGPUShader *m_QuadShader;
    WebGPUTexture *m_DepthTexture;

    RendererData m_RendererData;
    static RendererStats s_Stats;
};


