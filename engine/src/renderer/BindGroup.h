#pragma once

#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
namespace Figment
{
    class BindGroup
    {
    public:
        BindGroup(WGPUDevice device, WGPUShaderStageFlags visibility);
        ~BindGroup();
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
            BindSampler(texture.GetSampler());
        }
        WGPUBindGroup Get();
        WGPUBindGroupLayout GetLayout();

    private:
        void Build();
        void Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type);
        void BindTexture(WGPUTextureView textureView, uint64_t size);
        void BindSampler(WGPUSampler sampler);

        WGPUDevice m_Device = nullptr;
        WGPUShaderStageFlags m_Visibility;
        WGPUBindGroup m_BindGroup = nullptr;
        WGPUBindGroupLayout m_BindGroupLayout = nullptr;
        std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
        std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
    };
}
