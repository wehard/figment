#include "ComputePass.h"
#include "WebGPUCommand.h"

namespace Figment
{

    ComputePass::ComputePass(WGPUDevice device, WebGPUShader *shader) :
            m_Device(device), m_Shader(shader)
    {
    }

    ComputePass::ComputePass(WGPUDevice device, ComputePipeline *pipeline, BindGroup *bindGroup) :
            m_Device(device), m_Pipeline(pipeline), m_BindGroup(bindGroup)
    {
    }

    void ComputePass::Begin()
    {
        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(m_Device, "ComputeCommandEncoder");

        WGPUComputePassDescriptor computePassDesc = {};
        computePassDesc.label = "ComputePass";
        computePassDesc.timestampWrites = nullptr;

        m_ComputePassEncoder = wgpuCommandEncoderBeginComputePass(m_CommandEncoder, &computePassDesc);
    }

    void ComputePass::Dispatch(const std::string &name, uint32_t invocationCountX)
    {
        wgpuComputePassEncoderSetPipeline(m_ComputePassEncoder, m_Pipeline->Pipeline);
        wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, 0, m_BindGroup->Group, 0, nullptr);

        wgpuComputePassEncoderDispatchWorkgroups(m_ComputePassEncoder, invocationCountX, 1, 1);
    }

    void ComputePass::End()
    {
        wgpuComputePassEncoderEnd(m_ComputePassEncoder);

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "ComputeCommandBuffer");
        WebGPUCommand::SubmitCommandBuffer(m_Device, commandBuffer);

        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuComputePassEncoderRelease(m_ComputePassEncoder);

        m_CommandEncoder = nullptr;
        m_ComputePassEncoder = nullptr;
    }

    void ComputePass::Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type)
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

    void ComputePass::BindTexture(WGPUTextureView textureView, uint64_t size)
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

    void ComputePass::BindSampler(WGPUSampler sampler)
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
