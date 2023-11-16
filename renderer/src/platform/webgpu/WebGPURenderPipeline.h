#pragma once

#include "WebGPUContext.h"
#include "WebGPUShader.h"
#include "WebGPUBuffer.h"
#include "webgpu/webgpu.h"

class WebGPURenderPipeline
{
public:
    WebGPURenderPipeline(WebGPUContext &context, WebGPUShader &shader, WGPUVertexBufferLayout vertexBufferLayout);
    ~WebGPURenderPipeline();

    WebGPURenderPipeline &SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat, WGPUFrontFace frontFace,
            WGPUCullMode cullMode);
    WebGPURenderPipeline &SetDepthStencilState(WGPUTextureFormat format, WGPUCompareFunction compareFunction, bool depthWriteEnabled);
    WebGPURenderPipeline &SetBinding(WGPUBindGroupLayoutEntry layoutEntry, WGPUBindGroupEntry bindGroupEntry);
    WebGPURenderPipeline &SetColorTargetStates(std::vector<WGPUColorTargetState> colorTargetStates);
    void Build();
    WGPURenderPipeline GetPipeline()
    { return m_Pipeline; }
    WGPUBindGroup GetBindGroup()
    { return m_BindGroup; }
private:
    WebGPUContext &m_Context;
    WebGPUShader &m_Shader;
    WGPURenderPipeline m_Pipeline = {};
    WGPUBindGroup m_BindGroup = {};
    WGPUPrimitiveState m_PrimitiveState = {};
    WGPUDepthStencilState m_DepthStencilState = {};
    std::vector<WGPUBlendState> m_BlendStates;
    std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
    std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
    WGPUVertexBufferLayout m_VertexBufferLayout = {};
    std::vector<WGPUColorTargetState> m_ColorTargetStates;
};
