#pragma once

#include "WebGPUContext.h"
#include "WebGPUShader.h"
#include "WebGPUBuffer.h"
#include "webgpu/webgpu.h"
#include "WebGPURenderer.h"

class WebGPURenderPipelineBuilder
{
public:
    explicit WebGPURenderPipelineBuilder(WebGPUContext &context, WebGPUShader &shader)
            : m_Context(context), m_Shader(shader)
    { }
    ~WebGPURenderPipelineBuilder();

    // void SetVertexBufferLayout(std::vector<WGPUVertexAttribute> attributes, uint64_t stride,
    //         WGPUVertexStepMode stepMode);
    void SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat, WGPUFrontFace frontFace,
            WGPUCullMode cullMode);
    // void AddColorTargetState(uint32_t index, WGPUTextureFormat format, WGPUColorWriteMask writeMask,
    //         bool noBlend = false);
    void SetDepthStencilState(WGPUTextureFormat format, WGPUCompareFunction compareFunction, bool depthWriteEnabled);
    void AddBindGroupLayoutEntry(uint32_t binding, WGPUBufferBindingType type, WGPUShaderStageFlags visibility,
            uint64_t minBindingSize);
    void AddBinding(uint32_t binding, WGPUBuffer buffer, uint64_t offset, uint64_t size);
    void Bind(WGPUBindGroupLayoutEntry layoutEntry, WGPUBindGroupEntry bindGroupEntry);
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
    // WGPUVertexBufferLayout m_VertexBufferLayout = {};
    // std::vector<WGPUVertexAttribute> m_VertexAttributes;
    WGPUPrimitiveState m_PrimitiveState = {};
    WGPUDepthStencilState m_DepthStencilState = {};
    std::vector<WGPUBlendState> m_BlendStates;
    std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
    std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
    WGPUBlendState m_BlendState = {};
};
