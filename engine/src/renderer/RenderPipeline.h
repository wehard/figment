#pragma once

#include <webgpu/webgpu.h>
#include "BindGroup.h"
#include "WebGPUShader.h"

namespace Figment
{
    class RenderPipeline
    {
    public:
        RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
                WGPUVertexBufferLayout vertexBufferLayout);
        RenderPipeline(WGPUDevice device, WebGPUShader &shader, BindGroup &bindGroup,
                std::vector<WGPUVertexBufferLayout> vertexBufferLayouts);
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
        std::vector<WGPUVertexBufferLayout> m_VertexBufferLayouts;
        WGPUPrimitiveState m_PrimitiveState;
        std::vector<WGPUColorTargetState> m_ColorTargetStates;
        std::vector<WGPUBlendState> m_BlendStates;
        WGPUDepthStencilState m_DepthStencilState;
    };

}
