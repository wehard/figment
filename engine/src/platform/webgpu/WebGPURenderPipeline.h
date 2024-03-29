#pragma once

#include "WebGPUContext.h"
#include "WebGPUShader.h"
#include "WebGPUBuffer.h"
#include "webgpu/webgpu.h"

namespace Figment
{
    class WebGPURenderPipeline
    {
    public:
        WebGPURenderPipeline(WGPUDevice device, WebGPUShader &shader, WGPUVertexBufferLayout vertexBufferLayout);
        ~WebGPURenderPipeline();

        WebGPURenderPipeline &SetPrimitiveState(WGPUPrimitiveTopology topology, WGPUIndexFormat stripIndexFormat,
                WGPUFrontFace frontFace,
                WGPUCullMode cullMode);
        WebGPURenderPipeline &SetDepthStencilState(WGPUTextureFormat format, WGPUCompareFunction compareFunction,
                bool depthWriteEnabled);
        WebGPURenderPipeline &SetBinding(WGPUBindGroupLayoutEntry layoutEntry, WGPUBindGroupEntry bindGroupEntry);
        WebGPURenderPipeline &SetColorTargetStates(std::vector<WGPUColorTargetState> colorTargetStates);
        void Build();
        WGPURenderPipeline Pipeline = {};
        WGPUBindGroup BindGroup = {};
    private:
        WGPUDevice m_Device;
        WebGPUShader &m_Shader;
        WGPUPrimitiveState m_PrimitiveState = {};
        WGPUDepthStencilState m_DepthStencilState = {};
        std::vector<WGPUBlendState> m_BlendStates;
        std::vector<WGPUBindGroupLayoutEntry> m_BindGroupLayoutEntries;
        std::vector<WGPUBindGroupEntry> m_BindGroupEntries;
        WGPUVertexBufferLayout m_VertexBufferLayout = {};
        std::vector<WGPUColorTargetState> m_ColorTargetStates;
    };
}
