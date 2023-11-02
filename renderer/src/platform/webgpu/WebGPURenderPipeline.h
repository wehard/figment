#pragma once

#include "WebGPUContext.h"
#include "WebGPUShader.h"
#include "webgpu/webgpu.h"

class WebGPURenderPipelineBuilder
{
public:
    explicit WebGPURenderPipelineBuilder(WebGPUContext &context, WebGPUShader &shader) : m_Context(context), m_Shader(shader)
    { }
    ~WebGPURenderPipelineBuilder() = default;

    WebGPURenderPipelineBuilder &SetVertexBufferLayout(std::vector<WGPUVertexAttribute> attributes, uint64_t stride,
            WGPUVertexStepMode stepMode)
    {
        m_VertexAttributes = attributes;
        m_VertexBufferLayout.attributeCount = m_VertexAttributes.size();
        m_VertexBufferLayout.attributes = m_VertexAttributes.data();
        m_VertexBufferLayout.arrayStride = stride;
        m_VertexBufferLayout.stepMode = stepMode;
        return *this;
    }

    void SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat, WGPUFrontFace frontFace, WGPUCullMode cullMode)
    {
        m_PrimitiveState = {
                .nextInChain = nullptr,
                .topology = topology,
                .stripIndexFormat = stripIndexFormat,
                .frontFace = frontFace,
                .cullMode = cullMode
        };
    }

    void AddColorTargetState(uint32_t index, WGPUTextureFormat format, WGPUColorWriteMask writeMask, bool noBlend = false)
    {
        if (m_ColorTargetStates.size() <= index)
            m_ColorTargetStates.resize(index + 1);

        m_BlendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        m_BlendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        m_BlendState.color.operation = WGPUBlendOperation_Add;
        m_BlendState.alpha.srcFactor = WGPUBlendFactor_One;
        m_BlendState.alpha.dstFactor = WGPUBlendFactor_One;
        m_BlendState.alpha.operation = WGPUBlendOperation_Add;

        m_ColorTargetStates[index] = {
                .nextInChain = nullptr,
                .format = format,
                .blend = noBlend ? nullptr : &m_BlendState,
                .writeMask = writeMask
        };
    }

    static WGPUDepthStencilState GetDefaultDepthStencilState()
    {
        WGPUDepthStencilState depthStencilState = {};
        depthStencilState.nextInChain = nullptr;
        depthStencilState.format = WGPUTextureFormat_Undefined;
        depthStencilState.depthWriteEnabled = false;
        depthStencilState.depthCompare = WGPUCompareFunction_Always;
        depthStencilState.stencilReadMask = 0xFFFFFFFF;
        depthStencilState.stencilWriteMask = 0xFFFFFFFF;
        depthStencilState.depthBias = 0;
        depthStencilState.depthBiasSlopeScale = 0.0;
        depthStencilState.depthBiasClamp = 0.0;
        depthStencilState.stencilFront = {
                .compare = WGPUCompareFunction_Always,
                .failOp = WGPUStencilOperation_Keep,
                .depthFailOp = WGPUStencilOperation_Keep,
                .passOp = WGPUStencilOperation_Keep,
        };
        depthStencilState.stencilBack = {
                .compare = WGPUCompareFunction_Always,
                .failOp = WGPUStencilOperation_Keep,
                .depthFailOp = WGPUStencilOperation_Keep,
                .passOp = WGPUStencilOperation_Keep,
        };
        return depthStencilState;
    }

    void SetDepthStencilState(WGPUTextureFormat format, WGPUCompareFunction compareFunction, bool depthWriteEnabled)
    {
        m_DepthStencilState = GetDefaultDepthStencilState();
        m_DepthStencilState.depthCompare = compareFunction;
        m_DepthStencilState.depthWriteEnabled = depthWriteEnabled;
        m_DepthStencilState.format = format;
        m_DepthStencilState.stencilReadMask = 0;
        m_DepthStencilState.stencilWriteMask = 0;
    }

    void AddBindGroupLayoutEntry(uint32_t binding, WGPUBufferBindingType type, WGPUShaderStageFlags visibility, uint64_t minBindingSize)
    {
        WGPUBindGroupLayoutEntry entry = {};
        entry.binding = binding;
        entry.visibility = visibility;
        entry.buffer.type = type;
        entry.buffer.minBindingSize = minBindingSize;
        m_BindGroupLayoutEntries.push_back(entry);
    }

    void AddBinding(uint32_t binding, WGPUBuffer buffer, uint64_t offset, uint64_t size)
    {
        WGPUBindGroupEntry entry = {
                .nextInChain = nullptr,
                .binding = binding,
                .buffer = buffer,
                .offset = offset,
                .size = size
        };
        m_BindGroupEntries.push_back(entry);
    }

    void Create()
    {
        WGPURenderPipelineDescriptor desc = {};
        desc.nextInChain = nullptr;
        desc.vertex = {
                .module = m_Shader.GetShaderModule(),
                .entryPoint = "vs_main",
                .constantCount = 0,
                .constants = nullptr,
        };
        desc.vertex.bufferCount = 1;
        desc.vertex.buffers = &m_VertexBufferLayout;
        desc.primitive = m_PrimitiveState;

        WGPUFragmentState fragmentState = {};
        fragmentState.module = m_Shader.GetShaderModule();
        fragmentState.entryPoint = "fs_main";
        fragmentState.targetCount = m_ColorTargetStates.size();
        fragmentState.targets = m_ColorTargetStates.data();
        desc.fragment = &fragmentState;
        desc.depthStencil = &m_DepthStencilState;

        desc.multisample.count = 1;
        desc.multisample.mask = ~0u;
        desc.multisample.alphaToCoverageEnabled = false;

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.nextInChain = nullptr;
        bindGroupLayoutDesc.entryCount = m_BindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();
        auto bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Context.GetDevice(), &bindGroupLayoutDesc);

        WGPUPipelineLayoutDescriptor layoutDesc = {
                .nextInChain = nullptr,
                .bindGroupLayoutCount = m_BindGroupLayoutEntries.size(),
                .bindGroupLayouts = &bindGroupLayout
        };

        desc.layout = wgpuDeviceCreatePipelineLayout(m_Context.GetDevice(), &layoutDesc);

        m_Pipeline =  wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &desc);

        WGPUBindGroupDescriptor bindGroupDesc = {
                .nextInChain = nullptr,
                .layout = bindGroupLayout,
                .entryCount = m_BindGroupEntries.size(),
                .entries = m_BindGroupEntries.data()
        };
        m_BindGroup = wgpuDeviceCreateBindGroup(m_Context.GetDevice(), &bindGroupDesc);
    }

    WGPURenderPipeline GetPipeline() { return m_Pipeline; }
    WGPUBindGroup GetBindGroup() { return m_BindGroup; }

private:
    WebGPUContext &m_Context;
    WebGPUShader &m_Shader;
    WGPURenderPipeline m_Pipeline = {};
    WGPUBindGroup m_BindGroup = {};
    WGPUVertexBufferLayout m_VertexBufferLayout = {};
    std::vector<WGPUVertexAttribute> m_VertexAttributes;
    WGPUPrimitiveState m_PrimitiveState = {};
    // WGPUFragmentState m_FragmentState = {};
    WGPUDepthStencilState m_DepthStencilState = {};
    std::vector<WGPUColorTargetState> m_ColorTargetStates;
    std::vector<WGPUBlendState> m_BlendStates;
    std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
    std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
    WGPUBlendState m_BlendState = {};
};
