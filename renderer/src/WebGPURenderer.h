#pragma once

#include "WebGPUContext.h"
#include "WebGPUBuffer.h"
#include "webgpu/webgpu.h"
#include "glm/glm.hpp"

class WebGPURenderer
{
public:
    explicit WebGPURenderer(WebGPUContext &context);
    WGPURenderPassEncoder Begin();
    void End();
    void DrawQuad(glm::mat4 transform, glm::vec4 color);

private:
    WebGPUContext &m_Context;
    WGPUCommandEncoder m_CommandEncoder;
    WGPURenderPassEncoder m_RenderPass;
    WGPUShaderModule m_ShaderModule;
    WebGPUVertexBuffer *m_VertexBuffer;
};
