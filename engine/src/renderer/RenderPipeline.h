#pragma once

#include <webgpu/webgpu.h>
#include "BindGroup.h"
#include "WebGPUShader.h"

namespace Figment
{
    struct ColorTargetState
    {
        WGPUTextureFormat Format;
        WGPUColorWriteMask WriteMask;
    };

    struct DepthStencilState
    {
        WGPUTextureFormat Format;
        bool DepthWriteEnabled;
        WGPUCompareFunction DepthCompare;
    };

    struct RenderPipelineDescriptor
    {
        WebGPUShader &Shader;
        BindGroup &BindGroup;
        std::vector<WGPUVertexBufferLayout> VertexBufferLayouts = {};
        std::vector<ColorTargetState> ColorTargetStates = {};
        std::vector<DepthStencilState> DepthStencilStates = {};
        WGPUPrimitiveState PrimitiveState = {
                .topology = WGPUPrimitiveTopology_TriangleList,
                .stripIndexFormat = WGPUIndexFormat_Undefined,
                .frontFace = WGPUFrontFace_CCW,
                .cullMode = WGPUCullMode_None,
        };
        WGPUMultisampleState MultisampleState = {
                .count = 1,
                .mask = 0xFFFFFFFF,
                .alphaToCoverageEnabled = false,
        };
    };

    class RenderPipeline
    {
    public:
        RenderPipeline(WGPUDevice device, const RenderPipelineDescriptor &&descriptor);
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
        void SetMultisampleState(uint32_t count, uint32_t mask, bool alphaToCoverageEnabled);
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
        WGPUMultisampleState m_MultisampleState = {
                .count = 1,
                .mask = 0xFFFFFFFF,
                .alphaToCoverageEnabled = false,
        };
    };

}
