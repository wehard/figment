#pragma once

#include "WebGPUContext.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "Camera.h"
#include "webgpu/webgpu.h"
#include "glm/glm.hpp"

struct RenderData
{
    glm::mat4 ModelMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::vec4 Color;
};

struct RenderPassData
{
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

class WebGPURenderer
{
public:
    explicit WebGPURenderer(WebGPUContext &context);
    WGPURenderPassEncoder Begin(Camera &camera);
    void End();
    void DrawQuad(glm::mat4 transform, glm::vec4 color);
    uint32_t ReadPixel(int x, int y);

private:
    WebGPUContext &m_Context;
    WGPUCommandEncoder m_CommandEncoder;
    WGPURenderPassEncoder m_RenderPass;
    WGPUShaderModule m_ShaderModule;
    WebGPUVertexBuffer *m_VertexBuffer;
    RenderPassData m_RenderPassData;
    WebGPUTexture *m_IdTexture;
    WGPUBuffer m_PixelBuffer;
};