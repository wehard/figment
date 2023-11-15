#include "WebGPURenderPipeline.h"

WebGPURenderPipeline::WebGPURenderPipeline(WebGPUContext &context, WebGPUShader &shader)
        : m_Context(context), m_Shader(shader)
{ }

WebGPURenderPipeline::~WebGPURenderPipeline()
{
    wgpuRenderPipelineRelease(m_Pipeline);
    wgpuBindGroupRelease(m_BindGroup);
}

void WebGPURenderPipeline::SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat,
        WGPUFrontFace frontFace, WGPUCullMode cullMode)
{
    m_PrimitiveState = {
            .nextInChain = nullptr,
            .topology = topology,
            .stripIndexFormat = stripIndexFormat,
            .frontFace = frontFace,
            .cullMode = cullMode
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

void WebGPURenderPipeline::SetDepthStencilState(WGPUTextureFormat format, WGPUCompareFunction compareFunction,
        bool depthWriteEnabled)
{
    m_DepthStencilState = GetDefaultDepthStencilState();
    m_DepthStencilState.depthCompare = compareFunction;
    m_DepthStencilState.depthWriteEnabled = depthWriteEnabled;
    m_DepthStencilState.format = format;
    m_DepthStencilState.stencilReadMask = 0;
    m_DepthStencilState.stencilWriteMask = 0;
}

void WebGPURenderPipeline::SetBinding(WGPUBindGroupLayoutEntry bindGroupLayoutEntry, WGPUBindGroupEntry bindGroupEntry)
{
    m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    m_BindGroupEntries.emplace_back(bindGroupEntry);
}

void WebGPURenderPipeline::Build()
{
    WGPURenderPipelineDescriptor desc = {};
    desc.nextInChain = nullptr;
    desc.vertex = m_Shader.GetVertexState();
    desc.primitive = m_PrimitiveState;

    WGPUFragmentState fragmentState = m_Shader.GetFragmentState();
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

    m_Pipeline = wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &desc);

    WGPUBindGroupDescriptor bindGroupDesc = {
            .nextInChain = nullptr,
            .layout = bindGroupLayout,
            .entryCount = m_BindGroupEntries.size(),
            .entries = m_BindGroupEntries.data()
    };
    m_BindGroup = wgpuDeviceCreateBindGroup(m_Context.GetDevice(), &bindGroupDesc);
    wgpuBindGroupLayoutRelease(bindGroupLayout);
}



