#include "RenderPipeline.h"

#include <utility>

namespace Figment
{
    RenderPipeline::RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
            WGPUVertexBufferLayout vertexBufferLayout) : m_Device(device), m_Shader(shader), m_BindGroup(bindGroup)
    {
        m_VertexBufferLayouts.emplace_back(vertexBufferLayout);
    }

    RenderPipeline::RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
            std::vector<WGPUVertexBufferLayout> vertexBufferLayouts) : m_Device(device), m_Shader(shader),
            m_BindGroup(bindGroup), m_VertexBufferLayouts(std::move(vertexBufferLayouts))
    {
    }

    RenderPipeline::~RenderPipeline()
    {
    }

    void RenderPipeline::AddColorTarget(WGPUTextureFormat format, WGPUColorWriteMask writeMask)
    {
        WGPUBlendState defaultBlendState = {
                .color = {
                        .operation = WGPUBlendOperation_Add,
                        .srcFactor = WGPUBlendFactor_One,
                        .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
                },
                .alpha = {
                        .operation = WGPUBlendOperation_Add,
                        .srcFactor = WGPUBlendFactor_One,
                        .dstFactor = WGPUBlendFactor_OneMinusSrcAlpha,
                }
        };
        m_BlendStates.push_back(defaultBlendState);
        m_ColorTargetStates.push_back({
                .format = format,
                .blend = &m_BlendStates.back(),
                .writeMask = writeMask
        });
    }

    void RenderPipeline::SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat,
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

    void RenderPipeline::SetDepthStencilState(WGPUTextureFormat format, bool depthWriteEnabled,
            WGPUCompareFunction depthCompare)
    {
        WGPUDepthStencilState depthStencilState = GetDefaultDepthStencilState();
        depthStencilState.depthCompare = depthCompare;
        depthStencilState.depthWriteEnabled = depthWriteEnabled;
        depthStencilState.format = format;
        depthStencilState.stencilReadMask = 0;
        depthStencilState.stencilWriteMask = 0;
        m_DepthStencilState = depthStencilState;
    }

    WGPURenderPipeline RenderPipeline::Get()
    {
        if (m_RenderPipeline != nullptr)
            return m_RenderPipeline;

        WGPUVertexState vertexState = {};
        vertexState.nextInChain = nullptr;
        vertexState.entryPoint = m_Shader.GetVertexEntryPoint();
        vertexState.module = m_Shader.GetShaderModule();
        vertexState.constantCount = 0; // TODO: add support for constants
        vertexState.constants = nullptr;
        vertexState.bufferCount = m_VertexBufferLayouts.size();
        vertexState.buffers = m_VertexBufferLayouts.data();

        WGPUFragmentState fragmentState = {};
        fragmentState.nextInChain = nullptr;
        fragmentState.entryPoint = m_Shader.GetFragmentEntryPoint();
        fragmentState.module = m_Shader.GetShaderModule();
        fragmentState.constantCount = 0; // TODO: add support for constants
        fragmentState.constants = nullptr;
        fragmentState.targetCount = m_ColorTargetStates.size();
        fragmentState.targets = m_ColorTargetStates.data();

        auto bindGroupLayout = m_BindGroup.GetLayout();
        WGPUPipelineLayoutDescriptor layoutDesc = {
                .nextInChain = nullptr,
                .bindGroupLayoutCount = 1,
                .bindGroupLayouts = &bindGroupLayout,
        };

        m_RenderPipelineLayout = wgpuDeviceCreatePipelineLayout(m_Device, &layoutDesc);

        WGPURenderPipelineDescriptor renderPipelineDesc = {};
        renderPipelineDesc.nextInChain = nullptr;
        renderPipelineDesc.vertex = vertexState;
        renderPipelineDesc.primitive = m_PrimitiveState;
        renderPipelineDesc.fragment = &fragmentState;
        renderPipelineDesc.depthStencil = &m_DepthStencilState;
        renderPipelineDesc.multisample.count = 1;
        renderPipelineDesc.multisample.mask = ~0u;
        renderPipelineDesc.multisample.alphaToCoverageEnabled = false;
        renderPipelineDesc.layout = m_RenderPipelineLayout;
        m_RenderPipeline = wgpuDeviceCreateRenderPipeline(m_Device, &renderPipelineDesc);

        return m_RenderPipeline;
    }

}
