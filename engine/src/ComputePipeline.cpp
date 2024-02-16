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
    }
}
