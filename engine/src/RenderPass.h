#pragma once

#include "WebGPUShader.h"
#include "Mesh.h"
#include "WebGPUTexture.h"

#include <webgpu/webgpu.h>
#include <vector>

namespace Figment
{
    class RenderPass
    {
    public:
        RenderPass(WGPUDevice device);
        ~RenderPass() = default;
        void AddColorAttachment(WGPUTextureView textureView, WGPUTextureFormat format);
        void SetDepthStencilAttachment(WGPUTextureView textureView, WGPUTextureFormat format);
        void Begin();
        void End();
        void DrawIndexed(Mesh &mesh, glm::mat4 transform, WebGPUShader &shader);
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
    private:
        void Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type);
        void BindTexture(WGPUTextureView textureView, uint64_t size);
        void BindSampler(WGPUSampler sampler);

        WGPUDevice m_Device;
        WGPUCommandEncoder m_CommandEncoder = nullptr;
        WGPURenderPassEncoder m_RenderPassEncoder = nullptr;
        std::vector<WGPURenderPassColorAttachment> m_ColorAttachments;
        std::vector<WGPUColorTargetState> m_ColorTargetStates;
        WGPURenderPassDepthStencilAttachment m_DepthStencilAttachment;
        WGPUDepthStencilState m_DepthStencilState;

        std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
        std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
    };
}
