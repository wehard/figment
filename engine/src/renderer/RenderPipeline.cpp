#include "RenderPipeline.h"

#include <utility>

namespace Figment
{
    RenderPipeline::RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
            WGPUVertexBufferLayout vertexBufferLayout) : RenderPipeline(device, shader, bindGroup,
            std::vector<WGPUVertexBufferLayout> { vertexBufferLayout })
    {
        m_VertexBufferLayouts.emplace_back(vertexBufferLayout);
    }

    RenderPipeline::RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
            std::vector<WGPUVertexBufferLayout> vertexBufferLayouts) : m_Device(device), m_Shader(shader),
            m_BindGroup(bindGroup), m_VertexBufferLayouts(std::move(vertexBufferLayouts))
    {
    }

    RenderPipeline::RenderPipeline(WGPUDevice device, const RenderPipelineDescriptor &&descriptor) : m_Device(device),
            m_Shader(descriptor.Shader),
            m_BindGroup(descriptor.BindGroup), m_VertexBufferLayouts(descriptor.VertexBufferLayouts)
    {
        for (auto &colorTarget : descriptor.ColorTargetStates)
        {
            AddColorTarget(colorTarget.Format, colorTarget.WriteMask);
        }
        for (auto &depthStencil : descriptor.DepthStencilStates)
        {
            SetDepthStencilState(depthStencil.Format, depthStencil.DepthWriteEnabled, depthStencil.DepthCompare);
        }
        SetPrimitiveState(descriptor.PrimitiveState.topology, descriptor.PrimitiveState.stripIndexFormat,
                descriptor.PrimitiveState.frontFace, descriptor.PrimitiveState.cullMode);
        SetMultisampleState(descriptor.MultisampleState.count, descriptor.MultisampleState.mask,
                descriptor.MultisampleState.alphaToCoverageEnabled);
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
                .blend = nullptr, // &m_BlendStates.back(),
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

    void RenderPipeline::SetMultisampleState(uint32_t count, uint32_t mask = ~0u, bool alphaToCoverageEnabled = false)
    {
        m_MultisampleState = {
                .count = count,
                .mask = mask,
                .alphaToCoverageEnabled = alphaToCoverageEnabled,
        };
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
        renderPipelineDesc.multisample = m_MultisampleState;
        renderPipelineDesc.layout = m_RenderPipelineLayout;
        m_RenderPipeline = wgpuDeviceCreateRenderPipeline(m_Device, &renderPipelineDesc);

        return m_RenderPipeline;
    }
}
