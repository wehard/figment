#pragma once

#include "webgpu/webgpu.h"
#include <string>

namespace Figment
{
    class WebGPUShader
    {
    public:
        WebGPUShader(WGPUDevice device, const std::string &filePath, const char *label = "WebGPUShader");
        ~WebGPUShader();

        WGPUShaderModule GetShaderModule()
        { return m_ShaderModule; }
        const char *GetVertexEntryPoint()
        { return m_VertexEntryPoint; }
        const char *GetFragmentEntryPoint()
        { return m_FragmentEntryPoint; }

    private:
        WGPUShaderModule m_ShaderModule;
        const std::string &m_FilePath;

        const char *m_VertexEntryPoint = "vs_main";
        const char *m_FragmentEntryPoint = "fs_main";
    };
}
