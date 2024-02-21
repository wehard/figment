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
        auto layout = m_BindGroup.GetLayout();
        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
        pipelineLayoutDesc.label = "ComputePipelineLayout";
        pipelineLayoutDesc.bindGroupLayoutCount = 1;
        pipelineLayoutDesc.bindGroupLayouts = &layout;
        auto pipelineLayout = wgpuDeviceCreatePipelineLayout(m_Device, &pipelineLayoutDesc);

        WGPUComputePipelineDescriptor computePipelineDesc = {};
        computePipelineDesc.label = "ComputePipeline";
        computePipelineDesc.compute.entryPoint = entryPoint;
        computePipelineDesc.compute.module = shaderModule;
        computePipelineDesc.layout = pipelineLayout;
        Pipeline = wgpuDeviceCreateComputePipeline(m_Device, &computePipelineDesc);
    }
}
