#pragma once

#include "WebGPUBuffer.h"
#include "WebGPUShader.h"
#include "WebGPUTexture.h"
#include "webgpu/webgpu.h"
#include <vector>

namespace Figment
{
    class ComputePass
    {
    public:
        ComputePass(WGPUDevice device, WebGPUShader *shader);
        ~ComputePass() = default;
        void Begin();
        void Dispatch(const std::string &name, uint32_t invocationsX);
        void End();
        template<typename T>
        void Bind(WebGPUVertexBuffer<T> &buffer)
        {
            Bind(buffer.GetBuffer(), buffer.GetSize(), WGPUBufferBindingType_Storage);
        }
        template<typename T>
        void Bind(WebGPUUniformBuffer<T> &buffer)
        {
            Bind(buffer.GetBuffer(), buffer.GetSize(), WGPUBufferBindingType_Uniform);
        }
        void Bind(WebGPUTexture &texture)
        {
            BindTexture(texture.GetTextureView(), texture.GetWidth() * texture.GetHeight() * 4);
            // BindSampler(texture.GetSampler());
        }
    private:
        void Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type);
        void BindTexture(WGPUTextureView textureView, uint64_t size);
        void BindSampler(WGPUSampler sampler);
        std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
        std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
        WGPUDevice m_Device;
        WebGPUShader *m_Shader;
        WGPUCommandEncoder m_CommandEncoder;
        WGPUComputePassEncoder m_ComputePassEncoder;
    };
}
