#include "RenderPass.h"
#include "WebGPUCommand.h"
#include "WebGPURenderPipeline.h"

namespace Figment
{
    RenderPass::RenderPass(WGPUDevice device) : m_Device(device)
    {
        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(device, "RenderPassCommandEncoder");
    }

    void RenderPass::AddColorAttachment(WGPUTextureView textureView, WGPUTextureFormat format)
    {
        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.nextInChain = nullptr;
        colorAttachment.view = textureView;
        colorAttachment.depthSlice = 0;
        colorAttachment.resolveTarget = nullptr;
        colorAttachment.loadOp = WGPULoadOp_Load;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };

        m_ColorAttachments.push_back(colorAttachment);
        m_ColorTargetStates.push_back({
                .format = format,
                .blend = nullptr,
                .writeMask = WGPUColorWriteMask_All
        });
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

    void RenderPass::SetDepthStencilAttachment(WGPUTextureView depthStencilTextureView, WGPUTextureFormat format)
    {
        WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        depthStencilAttachment.view = depthStencilTextureView;
        depthStencilAttachment.depthClearValue = 1.0f;
        depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.depthReadOnly = false;
        depthStencilAttachment.stencilClearValue = 0;
        depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
        depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
        depthStencilAttachment.stencilReadOnly = true;
        m_DepthStencilAttachment = depthStencilAttachment;

        WGPUDepthStencilState depthStencilState = GetDefaultDepthStencilState();
        depthStencilState.depthCompare = WGPUCompareFunction_Less;
        depthStencilState.depthWriteEnabled = true;
        depthStencilState.format = format;
        depthStencilState.stencilReadMask = 0;
        depthStencilState.stencilWriteMask = 0;
        m_DepthStencilState = depthStencilState;
    }

    void RenderPass::Begin()
    {
        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = m_ColorAttachments.size();
        renderPassDesc.colorAttachments = m_ColorAttachments.data();
        renderPassDesc.depthStencilAttachment = &m_DepthStencilAttachment;

        m_RenderPassEncoder = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);
    }

    void RenderPass::End()
    {
        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "RenderPassCommandBuffer");

        WebGPUCommand::SubmitCommandBuffer(m_Device, commandBuffer);
        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuRenderPassEncoderRelease(m_RenderPassEncoder);
        m_CommandEncoder = nullptr;
        m_RenderPassEncoder = nullptr;
    }

    void RenderPass::DrawIndexed(Mesh &mesh, glm::mat4 transform, WebGPUShader &shader)
    {
        mesh.UniformBuffer()->SetData(&transform, sizeof(transform));

        WGPUPrimitiveState primitiveState = {};
        primitiveState.nextInChain = nullptr;
        primitiveState.topology = WGPUPrimitiveTopology_TriangleList;
        primitiveState.stripIndexFormat = WGPUIndexFormat_Undefined;
        primitiveState.frontFace = WGPUFrontFace_CCW;
        primitiveState.cullMode = WGPUCullMode_None;

        auto vertexBufferLayout = mesh.VertexBuffer()->GetVertexLayout();
        WGPUVertexState vertexState = {};
        vertexState.nextInChain = nullptr;
        vertexState.entryPoint = shader.GetVertexEntryPoint();
        vertexState.module = shader.GetShaderModule();
        vertexState.constantCount = 0; // TODO: add support for constants
        vertexState.constants = nullptr;
        vertexState.bufferCount = 1;
        vertexState.buffers = &vertexBufferLayout;

        WGPUFragmentState fragmentState = {};
        fragmentState.nextInChain = nullptr;
        fragmentState.entryPoint = shader.GetFragmentEntryPoint();
        fragmentState.module = shader.GetShaderModule();
        fragmentState.constantCount = 0; // TODO: add support for constants
        fragmentState.constants = nullptr;
        fragmentState.targetCount = m_ColorTargetStates.size();
        fragmentState.targets = m_ColorTargetStates.data();

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.nextInChain = nullptr;
        bindGroupLayoutDesc.entryCount = m_BindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();
        auto bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Device, &bindGroupLayoutDesc);

        WGPUBindGroupDescriptor bindGroupDesc = {
                .nextInChain = nullptr,
                .layout = bindGroupLayout,
                .entryCount = m_BindGroupEntries.size(),
                .entries = m_BindGroupEntries.data()
        };

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {
                .nextInChain = nullptr,
                .bindGroupLayoutCount = 1,
                .bindGroupLayouts = &bindGroupLayout
        };

        WGPURenderPipelineDescriptor renderPipelineDesc = {};
        renderPipelineDesc.nextInChain = nullptr;
        renderPipelineDesc.vertex = vertexState;
        renderPipelineDesc.primitive = primitiveState;
        renderPipelineDesc.fragment = &fragmentState;
        renderPipelineDesc.depthStencil = &m_DepthStencilState;
        renderPipelineDesc.multisample.count = 1;
        renderPipelineDesc.multisample.mask = ~0u;
        renderPipelineDesc.multisample.alphaToCoverageEnabled = false;
        renderPipelineDesc.layout = wgpuDeviceCreatePipelineLayout(m_Device, &pipelineLayoutDesc);

        auto bindGroup = wgpuDeviceCreateBindGroup(m_Device, &bindGroupDesc);
        auto pipeline = wgpuDeviceCreateRenderPipeline(m_Device, &renderPipelineDesc);

        WebGPUCommand::DrawIndexed(m_RenderPassEncoder, pipeline, bindGroup,
                *mesh.IndexBuffer(), *mesh.VertexBuffer(), mesh.IndexCount());
    }

    void RenderPass::Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type)
    {
        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.buffer = buffer;
        bindGroupEntry.offset = 0;
        bindGroupEntry.size = size;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
        bindGroupLayoutEntry.buffer.type = type;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }

    void RenderPass::BindTexture(WGPUTextureView textureView, uint64_t size)
    {
        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.size = size;
        bindGroupEntry.offset = 0;
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.textureView = textureView;
        bindGroupEntry.buffer = nullptr;
        bindGroupEntry.sampler = nullptr;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
        bindGroupLayoutEntry.texture.sampleType = WGPUTextureSampleType_Float;
        bindGroupLayoutEntry.texture.viewDimension = WGPUTextureViewDimension_2D;
        bindGroupLayoutEntry.texture.multisampled = false;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }

    void RenderPass::BindSampler(WGPUSampler sampler)
    {
        WGPUBindGroupEntry bindGroupEntry = GetDefaultWGPUBindGroupEntry();
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.sampler = sampler;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUSamplerBindingLayout samplerBindingLayout = {};
        samplerBindingLayout.nextInChain = nullptr;
        samplerBindingLayout.type = WGPUSamplerBindingType_Filtering;

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
        bindGroupLayoutEntry.sampler = samplerBindingLayout;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }
}
