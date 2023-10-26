#include "WebGPUShader.h"

static void CompilationInfoCallback(WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo *compilationInfo, void *userdata)
{
    printf("WebGPUShader compilation info: status %d, message oount: %zu\n", status, compilationInfo->messageCount);
    if (status == WGPUCompilationInfoRequestStatus_Success)
    {
        for (uint32_t i = 0; i < compilationInfo->messageCount; ++i)
        {
            const WGPUCompilationMessage &message = compilationInfo->messages[i];
            printf("WebGPUShader compilation info: %s\n", message.message);
        }
    }
    else
    {
        printf("WebGPUShader compilation info: failed to get compilation info\n");
    }
}

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

    // wgpuShaderModuleGetCompilationInfo(m_ShaderModule, CompilationInfoCallback, nullptr);
}

WebGPUShader::~WebGPUShader()
{
    wgpuShaderModuleRelease(m_ShaderModule);
}
