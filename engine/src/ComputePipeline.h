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

        // template<typename T>
        // void Bind(WebGPUVertexBuffer<T> &buffer)
        // {
        //     Bind(buffer.GetBuffer(), buffer.GetSize(), WGPUBufferBindingType_Storage);
        // }
        // template<typename T>
        // void Bind(WebGPUUniformBuffer<T> &buffer)
        // {
        //     Bind(buffer.GetBuffer(), buffer.GetSize(), WGPUBufferBindingType_Uniform);
        // }
        // void Bind(WebGPUTexture &texture)
        // {
        //     BindTexture(texture.GetTextureView(), texture.GetWidth() * texture.GetHeight() * 4);
        //     BindSampler(texture.GetSampler());
        // }

        WGPUComputePipeline Pipeline;
        // WGPUBindGroup BindGroup;
    private:
        // void Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type);
        // void BindTexture(WGPUTextureView textureView, uint64_t size);
        // void BindSampler(WGPUSampler sampler);

        WGPUDevice m_Device;
        BindGroup &m_BindGroup;
        // std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
        // std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
    };
}
