#pragma once

#include "WebGPUContext.h"
#include "WebGPUBuffer.h"
#include "Camera.h"
#include "webgpu/webgpu.h"
#include "glm/glm.hpp"

struct MVP
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
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

private:
    WebGPUContext &m_Context;
    WGPUCommandEncoder m_CommandEncoder;
    WGPURenderPassEncoder m_RenderPass;
    WGPUShaderModule m_ShaderModule;
    WebGPUVertexBuffer *m_VertexBuffer;
    WebGPUUniformBuffer<MVP> *m_UniformBuffer;
    RenderPassData m_RenderPassData;
};
