#pragma once

#include "WebGPUBuffer.h"
#include "ComputePipeline.h"
#include "webgpu/webgpu.h"
#include <vector>

namespace Figment
{
    class ComputePass
    {
    public:
        ComputePass(WGPUDevice device, ComputePipeline *pipeline, BindGroup *bindGroup);
        ~ComputePass() = default;
        void Begin();
        void Dispatch(const std::string &name, uint32_t workGroupCountX, uint32_t workGroupCountY = 1,
                uint32_t workGroupCountZ = 1);
        void End();
    private:
        WGPUDevice m_Device = nullptr;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WGPUComputePassEncoder m_ComputePassEncoder = nullptr;
        ComputePipeline *m_Pipeline = nullptr;
        BindGroup *m_BindGroup = nullptr;
    };
}
