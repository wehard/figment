#pragma once

#include <webgpu/webgpu.h>
#include "BindGroup.h"
#include "WebGPUShader.h"

namespace Figment
{
    class RenderTarget
    {
    public:
        RenderTarget() = default;
        RenderTarget(WGPUDevice device, WGPUTextureFormat format, WGPUTextureUsage usage,
                uint32_t width, uint32_t height)
        {

        }
        ~RenderTarget() = default;

        void AddColorAttachment(WGPUTextureView textureView, WGPUTextureFormat format)
        {
        }

        void SetDepthStencilAttachment(WGPUTextureView textureView, WGPUTextureFormat format)
        {
        }

    private:
        WGPUTexture m_Texture = nullptr;
        WGPUTextureView m_TextureView = nullptr;
        std::vector<WGPURenderPassColorAttachment> m_ColorAttachments;
        WGPURenderPassDepthStencilAttachment m_DepthStencilAttachment;
    };

    class RenderPipeline
    {
    public:
        RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
                WGPUVertexBufferLayout vertexBufferLayout);
        ~RenderPipeline();
        void AddColorTarget(WGPUTextureFormat format, WGPUColorWriteMask writeMask);
        void SetDepthStencilState(WGPUTextureFormat format, bool depthWriteEnabled = true,
                WGPUCompareFunction depthCompare = WGPUCompareFunction_Less);
        void SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat,
                WGPUFrontFace frontFace, WGPUCullMode cullMode);
        WGPURenderPipeline Get();

    private:
        WGPUDevice m_Device = nullptr;
        WGPURenderPipeline m_RenderPipeline = nullptr;
        WGPUPipelineLayout m_RenderPipelineLayout = nullptr;
        WebGPUShader &m_Shader;
        BindGroup &m_BindGroup;
        WGPUVertexBufferLayout m_VertexBufferLayout;
        WGPUPrimitiveState m_PrimitiveState;
        std::vector<WGPUColorTargetState> m_ColorTargetStates;
        WGPUDepthStencilState m_DepthStencilState;
    };

}
