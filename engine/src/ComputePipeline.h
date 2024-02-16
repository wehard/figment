#pragma once

#include "webgpu/webgpu.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "BindGroup.h"
#include <vector>

namespace Figment
{
    class ComputePipeline
    {
    public:
        ComputePipeline(WGPUDevice device, BindGroup &bindGroup);
        void Build(const char *entryPoint, WGPUShaderModule shaderModule);
        ~ComputePipeline();

        WGPUComputePipeline Pipeline;
    private:
        WGPUDevice m_Device;
        BindGroup &m_BindGroup;
    };
}
