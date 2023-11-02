#pragma once

#include "webgpu/webgpu.h"
#include <string>

class WebGPUShader
{
public:
    WebGPUShader(WGPUDevice device, const std::string &shaderSource, const char *label = "Shader Module");
    ~WebGPUShader();

    WGPUShaderModule GetShaderModule() { return m_ShaderModule; }
    std::string GetShaderSource() { return m_ShaderSource; }
    WGPUVertexState GetVertexStage();
    WGPUFragmentState GetFragmentStage(WGPUTextureFormat textureFormat, WGPUTextureFormat idFormat);
    WGPUPipelineLayout GetPipelineLayout(uint64_t uniformSize);
    const WGPUDepthStencilState GetDepthStencilState(WGPUTextureFormat depthFormat);
private:
    WGPUDevice m_Device;
    WGPUShaderModule m_ShaderModule;
    std::string m_ShaderSource;

    WGPUVertexAttribute m_VertexAttribute;
    WGPUVertexBufferLayout m_VertexBufferLayout;
    WGPUColorTargetState m_ColorTargets[2] = {};
    WGPUBlendState m_BlendState = {};
    WGPUDepthStencilState m_DepthStencilState = {};
};

