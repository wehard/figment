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

constexpr uint32_t MaxCircleCount = 100;

struct CircleVertex
{
    glm::vec3 Position;
    glm::vec4 Color;
    int32_t Id;
};

struct RendererData
{
    std::vector<CircleVertex> CircleVertices;
    uint32_t CircleVertexCount = 0;

    void Init()
    {
        CircleVertices.resize(MaxCircleCount * 4);
    }

    void Reset()
    {
        CircleVertexCount = 0;
    }
};

class WebGPURenderer
{
public:
    explicit WebGPURenderer(WebGPUContext &context);
    WGPURenderPassEncoder Begin(Camera &camera);
    void End();
    void DrawQuad(glm::mat4 transform, glm::vec4 color, int32_t id);
    void DrawCircle(glm::vec3 position, glm::vec4 color, int32_t id);
    void ReadPixel(int x, int y, std::function<void(int32_t)> callback);
    void OnResize(uint32_t width, uint32_t height);
    WebGPUShader *GetShader() { return m_Shader; }
private:
    void DrawCircles();
    WebGPUContext &m_Context;
    WGPUCommandEncoder m_CommandEncoder = {};
    WGPURenderPassEncoder m_RenderPass = {};
    WebGPUVertexBuffer *m_VertexBuffer;
    CameraData m_CameraData = {};
    WebGPUTexture *m_IdTexture;
    WebGPUBuffer<std::byte> *m_PixelBuffer;
    WebGPUShader *m_Shader;
    WebGPUShader *m_CircleShader;
    WebGPUTexture *m_DepthTexture;

    RendererData m_RendererData;
};


