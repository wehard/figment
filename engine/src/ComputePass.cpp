#include "ComputePass.h"
#include "WebGPUCommand.h"

namespace Figment
{

    ComputePass::ComputePass(WGPUDevice device, WebGPUShader &shader) :
            m_Device(device), m_Shader(shader)
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
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.label = "ComputeBindGroupLayout";
        bindGroupLayoutDesc.entryCount = m_BindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();

        auto bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Device, &bindGroupLayoutDesc);

        WGPUBindGroupDescriptor bindGroupDesc = {};
        bindGroupDesc.label = "ComputeBindGroup";
        bindGroupDesc.layout = bindGroupLayout;
        bindGroupDesc.entryCount = m_BindGroupEntries.size();
        bindGroupDesc.entries = m_BindGroupEntries.data();

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
        pipelineLayoutDesc.label = "ComputePipelineLayout";
        pipelineLayoutDesc.bindGroupLayoutCount = 1;
        pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
        auto pipelineLayout = wgpuDeviceCreatePipelineLayout(m_Device, &pipelineLayoutDesc);

        WGPUComputePipelineDescriptor computePipelineDesc = {};
        computePipelineDesc.label = "ComputePipeline";
        computePipelineDesc.compute.entryPoint = name.c_str();
        computePipelineDesc.compute.module = m_Shader.GetShaderModule();
        computePipelineDesc.layout = pipelineLayout;

        auto pipeline = wgpuDeviceCreateComputePipeline(m_Device, &computePipelineDesc);
        auto bindGroup = wgpuDeviceCreateBindGroup(m_Device, &bindGroupDesc);
        wgpuComputePassEncoderSetPipeline(m_ComputePassEncoder, pipeline);
        wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, 0, bindGroup, 0, nullptr);

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

    void ComputePass::Bind(WGPUBuffer buffer, uint32_t size)
    {
        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.nextInChain = nullptr;
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.buffer = buffer;
        bindGroupEntry.offset = 0;
        bindGroupEntry.size = size;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
        bindGroupLayoutEntry.buffer.type = WGPUBufferBindingType_Storage;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }

    void ComputePass::BindUniform(WGPUBuffer buffer, uint32_t size)
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
        bindGroupLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }
}