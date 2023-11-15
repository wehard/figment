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

    void SetVertexBufferLayout(std::vector<WGPUVertexAttribute> attributes, uint64_t stride,
            WGPUVertexStepMode stepMode);
    WGPUVertexState GetVertexState();
    WGPUFragmentState GetFragmentState();
    void SetColorTargetStates(std::vector<WGPUColorTargetState> colorTargetStates);

private:
    WGPUDevice m_Device;
    WGPUShaderModule m_ShaderModule;
    std::string m_ShaderSource;

    WGPUVertexAttribute m_VertexAttribute;
    WGPUVertexBufferLayout m_VertexBufferLayout;
    WGPUColorTargetState m_ColorTargets[2] = {};
    WGPUBlendState m_BlendState = {};
    WGPUDepthStencilState m_DepthStencilState = {};

    const char *m_VertexEntryPoint = "vs_main";
    const char *m_FragmentEntryPoint = "fs_main";
    std::vector<WGPUVertexAttribute> m_VertexAttributes;
    std::vector<WGPUColorTargetState> m_ColorTargetStates;
};

