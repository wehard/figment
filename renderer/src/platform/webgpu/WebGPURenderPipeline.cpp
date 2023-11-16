#include "WebGPURenderPipeline.h"

WebGPURenderPipeline::WebGPURenderPipeline(WebGPUContext &context, WebGPUShader &shader, WGPUVertexBufferLayout vertexBufferLayout)
        : m_Context(context), m_Shader(shader), m_VertexBufferLayout(vertexBufferLayout)
{ }

WebGPURenderPipeline::~WebGPURenderPipeline()
{
    wgpuRenderPipelineRelease(m_Pipeline);
    wgpuBindGroupRelease(m_BindGroup);
}

WebGPURenderPipeline &WebGPURenderPipeline::SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat,
        WGPUFrontFace frontFace, WGPUCullMode cullMode)
{
    m_PrimitiveState = {
            .nextInChain = nullptr,
            .topology = topology,
            .stripIndexFormat = stripIndexFormat,
            .frontFace = frontFace,
            .cullMode = cullMode
    };
    return *this;
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

WebGPURenderPipeline &WebGPURenderPipeline::SetDepthStencilState(WGPUTextureFormat format, WGPUCompareFunction compareFunction,
        bool depthWriteEnabled)
{
    m_DepthStencilState = GetDefaultDepthStencilState();
    m_DepthStencilState.depthCompare = compareFunction;
    m_DepthStencilState.depthWriteEnabled = depthWriteEnabled;
    m_DepthStencilState.format = format;
    m_DepthStencilState.stencilReadMask = 0;
    m_DepthStencilState.stencilWriteMask = 0;
    return *this;
}

WebGPURenderPipeline &WebGPURenderPipeline::SetBinding(WGPUBindGroupLayoutEntry bindGroupLayoutEntry, WGPUBindGroupEntry bindGroupEntry)
{
    m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    m_BindGroupEntries.emplace_back(bindGroupEntry);
    return *this;
}

WebGPURenderPipeline &WebGPURenderPipeline::SetColorTargetStates(std::vector<WGPUColorTargetState> colorTargetStates)
{
    m_ColorTargetStates = colorTargetStates;
    return *this;
}

void WebGPURenderPipeline::Build()
{
    WGPUVertexState vertexState = {};
    vertexState.nextInChain = nullptr;
    vertexState.entryPoint = m_Shader.GetVertexEntryPoint();
    vertexState.module = m_Shader.GetShaderModule();
    vertexState.constantCount = 0; // TODO: add support for constants
    vertexState.constants = nullptr;
    vertexState.bufferCount = 1;
    vertexState.buffers = &m_VertexBufferLayout;

    WGPURenderPipelineDescriptor desc = {};
    desc.nextInChain = nullptr;
    desc.vertex = vertexState;
    desc.primitive = m_PrimitiveState;

    WGPUFragmentState fragmentState = {};
    fragmentState.nextInChain = nullptr;
    fragmentState.entryPoint = m_Shader.GetFragmentEntryPoint();
    fragmentState.module = m_Shader.GetShaderModule();
    fragmentState.constantCount = 0; // TODO: add support for constants
    fragmentState.constants = nullptr;
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
