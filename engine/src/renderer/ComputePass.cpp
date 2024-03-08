#include "ComputePass.h"
#include "WebGPUCommand.h"

namespace Figment
{
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

    void ComputePass::Dispatch(const std::string &name, uint32_t workGroupCountX, uint32_t workGroupCountY,
            uint32_t workGroupCountZ)
    {
        wgpuComputePassEncoderSetPipeline(m_ComputePassEncoder, m_Pipeline->Pipeline);
        wgpuComputePassEncoderSetBindGroup(m_ComputePassEncoder, 0, m_BindGroup->Get(), 0, nullptr);

        wgpuComputePassEncoderDispatchWorkgroups(m_ComputePassEncoder, workGroupCountX, workGroupCountY,
                workGroupCountZ);
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
}
