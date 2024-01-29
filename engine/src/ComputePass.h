#pragma once

#include "WebGPUBuffer.h"
#include "WebGPUShader.h"
#include "webgpu/webgpu.h"
#include <vector>

namespace Figment
{
    class ComputePass
    {
    public:
        ComputePass(WGPUDevice device, WebGPUShader &shader);
        ~ComputePass() = default;
        void Begin();
        void Dispatch(const std::string &name, uint32_t invocationsX);
        void End();
        void Bind(WGPUBuffer buffer, uint64_t size);
        void BindUniform(WGPUBuffer buffer, uint64_t size);
    private:
        std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
        std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
        WGPUDevice m_Device;
        WebGPUShader m_Shader;
        WGPUCommandEncoder m_CommandEncoder;
        WGPUComputePassEncoder m_ComputePassEncoder;
    };
}
