#pragma once

#include "webgpu/webgpu.h"
#include <string>

class WebGPUShader
{
public:
    WebGPUShader(WGPUDevice device, const std::string &shaderSource, const char *label = "Shader Module");
    ~WebGPUShader();

    WGPUShaderModule GetShaderModule() { return m_ShaderModule; }
private:
    WGPUShaderModule m_ShaderModule;
};

