#include "ComputePipeline.h"

namespace Figment
{

    ComputePipeline::ComputePipeline(WGPUDevice device, BindGroup &bindGroup)
            : m_Device(device), m_BindGroup(bindGroup)
    {
    }

    ComputePipeline::~ComputePipeline()
    {

    }

    void ComputePipeline::Build(const char *entryPoint, WGPUShaderModule shaderModule)
    {
        // WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        // bindGroupLayoutDesc.label = "ComputeBindGroupLayout";
        // bindGroupLayoutDesc.entryCount = m_BindGroupLayoutEntries.size();
        // bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();

        // auto bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Device, &bindGroupLayoutDesc);

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
        pipelineLayoutDesc.label = "ComputePipelineLayout";
        pipelineLayoutDesc.bindGroupLayoutCount = 1;
        pipelineLayoutDesc.bindGroupLayouts = &m_BindGroup.Layout;
        auto pipelineLayout = wgpuDeviceCreatePipelineLayout(m_Device, &pipelineLayoutDesc);

        WGPUComputePipelineDescriptor computePipelineDesc = {};
        computePipelineDesc.label = "ComputePipeline";
        computePipelineDesc.compute.entryPoint = entryPoint;
        computePipelineDesc.compute.module = shaderModule;
        computePipelineDesc.layout = pipelineLayout;
        Pipeline = wgpuDeviceCreateComputePipeline(m_Device, &computePipelineDesc);

        // WGPUBindGroupDescriptor bindGroupDesc = {};
        // bindGroupDesc.label = "ComputeBindGroup";
        // bindGroupDesc.layout = bindGroupLayout;
        // bindGroupDesc.entryCount = m_BindGroupEntries.size();
        // bindGroupDesc.entries = m_BindGroupEntries.data();
        // BindGroup = wgpuDeviceCreateBindGroup(m_Device, &bindGroupDesc);
    }

    // void ComputePipeline::Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type)
    // {
    //     WGPUBindGroupEntry bindGroupEntry = {};
    //     bindGroupEntry.binding = m_BindGroupEntries.size();
    //     bindGroupEntry.buffer = buffer;
    //     bindGroupEntry.offset = 0;
    //     bindGroupEntry.size = size;
    //
    //     m_BindGroupEntries.emplace_back(bindGroupEntry);
    //
    //     WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
    //     bindGroupLayoutEntry.nextInChain = nullptr;
    //     bindGroupLayoutEntry.binding = bindGroupEntry.binding;
    //     bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
    //     bindGroupLayoutEntry.buffer.type = type;
    //
    //     m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    // }

    // void ComputePipeline::BindTexture(WGPUTextureView textureView, uint64_t size)
    // {
    //     WGPUBindGroupEntry bindGroupEntry = {};
    //     bindGroupEntry.size = size;
    //     bindGroupEntry.offset = 0;
    //     bindGroupEntry.binding = m_BindGroupEntries.size();
    //     bindGroupEntry.textureView = textureView;
    //     bindGroupEntry.buffer = nullptr;
    //     bindGroupEntry.sampler = nullptr;
    //
    //     m_BindGroupEntries.emplace_back(bindGroupEntry);
    //
    //     WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
    //     bindGroupLayoutEntry.nextInChain = nullptr;
    //     bindGroupLayoutEntry.binding = bindGroupEntry.binding;
    //     bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
    //     bindGroupLayoutEntry.texture.sampleType = WGPUTextureSampleType_Float;
    //     bindGroupLayoutEntry.texture.viewDimension = WGPUTextureViewDimension_2D;
    //     bindGroupLayoutEntry.texture.multisampled = false;
    //
    //     m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    // }

    // void ComputePipeline::BindSampler(WGPUSampler sampler)
    // {
    //     WGPUBindGroupEntry bindGroupEntry = GetDefaultWGPUBindGroupEntry();
    //     bindGroupEntry.binding = m_BindGroupEntries.size();
    //     bindGroupEntry.sampler = sampler;
    //
    //     m_BindGroupEntries.emplace_back(bindGroupEntry);
    //
    //     WGPUSamplerBindingLayout samplerBindingLayout = {};
    //     samplerBindingLayout.nextInChain = nullptr;
    //     samplerBindingLayout.type = WGPUSamplerBindingType_Filtering;
    //
    //     WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
    //     bindGroupLayoutEntry.nextInChain = nullptr;
    //     bindGroupLayoutEntry.binding = bindGroupEntry.binding;
    //     bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
    //     bindGroupLayoutEntry.sampler = samplerBindingLayout;
    //
    //     m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    // }
}
