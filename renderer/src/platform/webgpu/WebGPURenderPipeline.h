#pragma once

#include "WebGPUContext.h"
#include "WebGPUShader.h"
#include "webgpu/webgpu.h"

class WebGPURenderPipeline
{
public:
    explicit WebGPURenderPipeline(WebGPUContext &context) : m_Context(context)
    { }
    ~WebGPURenderPipeline() = default;

    WebGPURenderPipeline &SetVertexBufferLayout(std::vector<WGPUVertexAttribute> attributes, uint64_t stride,
            WGPUVertexStepMode stepMode)
    {
        m_VertexAttributes = attributes;
        m_VertexBufferLayout.attributeCount = m_VertexAttributes.size();
        m_VertexBufferLayout.attributes = m_VertexAttributes.data();
        m_VertexBufferLayout.arrayStride = stride;
        m_VertexBufferLayout.stepMode = stepMode;
        return *this;
    }

    void SetVertexState(WGPUVertexState vertexState)
    {
        m_VertexState = vertexState;
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

    void SetFragmentState(WGPUFragmentState fragmentState)
    {
        m_FragmentState = fragmentState;
        m_FragmentState.constantCount = 0;
        m_FragmentState.constants = nullptr;
        m_FragmentState.targetCount = m_ColorTargetStates.size();
        m_FragmentState.targets = m_ColorTargetStates.data();
    }

    void SetDepthStencilState(WGPUDepthStencilState depthStencilState)
    {
        m_DepthStencilState = depthStencilState;
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

    WGPURenderPipeline Create()
    {
        WGPURenderPipelineDescriptor desc = {};
        desc.nextInChain = nullptr;
        desc.vertex = m_VertexState;
        desc.vertex.bufferCount = 1;
        desc.vertex.buffers = &m_VertexBufferLayout;
        desc.primitive = m_PrimitiveState;

        m_FragmentState.targetCount = m_ColorTargetStates.size();
        m_FragmentState.targets = m_ColorTargetStates.data();
        desc.fragment = &m_FragmentState;
        desc.depthStencil = &m_DepthStencilState;

        desc.multisample.count = 1;
        desc.multisample.mask = ~0u;
        desc.multisample.alphaToCoverageEnabled = false;

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.nextInChain = nullptr;
        bindGroupLayoutDesc.entryCount = m_BindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();
        m_BindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Context.GetDevice(), &bindGroupLayoutDesc);

        WGPUPipelineLayoutDescriptor layoutDesc = {
                .nextInChain = nullptr,
                .bindGroupLayoutCount = m_BindGroupLayoutEntries.size(),
                .bindGroupLayouts = &m_BindGroupLayout
        };

        desc.layout = wgpuDeviceCreatePipelineLayout(m_Context.GetDevice(), &layoutDesc);

        return wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &desc);
    }

    WGPUBindGroup CreateBindGroup()
    {
        WGPUBindGroupDescriptor bindGroupDesc = {
                .nextInChain = nullptr,
                .layout = m_BindGroupLayout,
                .entryCount = m_BindGroupEntries.size(),
                .entries = m_BindGroupEntries.data()
        };
        return wgpuDeviceCreateBindGroup(m_Context.GetDevice(), &bindGroupDesc);
    }

private:
    WebGPUContext &m_Context;
    WGPUVertexBufferLayout m_VertexBufferLayout = {};
    std::vector<WGPUVertexAttribute> m_VertexAttributes;
    WGPUPrimitiveState m_PrimitiveState = {};
    WGPUVertexState m_VertexState = {};
    WGPUFragmentState m_FragmentState = {};
    WGPUDepthStencilState m_DepthStencilState = {};
    std::vector<WGPUColorTargetState> m_ColorTargetStates;
    std::vector<WGPUBlendState> m_BlendStates;
    std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
    std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
    WGPUBindGroupLayout m_BindGroupLayout = {};
    WGPUBlendState m_BlendState = {};
};

class WebGPURenderPipelineBuilder
{
public:
    WebGPURenderPipelineBuilder(WebGPUContext &context)
            : m_Context(context)
    { }
    ~WebGPURenderPipelineBuilder()
    { }

    WGPURenderPipeline Create(WebGPUShader *shader)
    {
        WGPURenderPipelineDescriptor desc = {};
        desc.vertex = shader->GetVertexStage();
        auto fragment = shader->GetFragmentStage(m_Context.GetTextureFormat(), WGPUTextureFormat_R32Sint);
        desc.fragment = &fragment;
        desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        desc.primitive.cullMode = WGPUCullMode_None;
        desc.primitive.frontFace = WGPUFrontFace_CCW;
        desc.layout = shader->GetPipelineLayout(0);
        auto depthState = shader->GetDepthStencilState(WGPUTextureFormat_Depth24Plus);
        desc.depthStencil = &depthState;
        return wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &desc);
    }
private:
    WebGPUContext &m_Context;
};
