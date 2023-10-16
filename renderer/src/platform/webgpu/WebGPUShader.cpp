#include "WebGPUShader.h"

WebGPUShader::WebGPUShader(WGPUDevice device, const std::string &shaderSource, const char *label) : m_ShaderSource(shaderSource)
{
    WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {};
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = shaderSource.c_str();

    WGPUShaderModuleDescriptor shaderDesc = {};
    shaderDesc.nextInChain = &shaderCodeDesc.chain;

    m_ShaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

    wgpuShaderModuleSetLabel(m_ShaderModule, label);
}

WebGPUShader::~WebGPUShader()
{
    wgpuShaderModuleRelease(m_ShaderModule);
}
