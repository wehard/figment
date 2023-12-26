#pragma once

#include "webgpu/webgpu.h"
#include <string>

namespace Figment
{
    class WebGPUShader
    {
    public:
        WebGPUShader(WGPUDevice device, const std::string &shaderSource, const char *label = "Shader Module");
        ~WebGPUShader();

        WGPUShaderModule GetShaderModule()
        { return m_ShaderModule; }
        std::string GetShaderSource()
        { return m_ShaderSource; }
        const char *GetVertexEntryPoint()
        { return m_VertexEntryPoint; }
        const char *GetFragmentEntryPoint()
        { return m_FragmentEntryPoint; }

    private:
        WGPUShaderModule m_ShaderModule;
        std::string m_ShaderSource;

        const char *m_VertexEntryPoint = "vs_main";
        const char *m_FragmentEntryPoint = "fs_main";
    };
}
