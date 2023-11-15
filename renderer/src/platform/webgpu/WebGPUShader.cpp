#include "WebGPUShader.h"

static void CompilationInfoCallback(WGPUCompilationInfoRequestStatus status, const WGPUCompilationInfo *compilationInfo,
        void *userdata)
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

static WGPUBindGroupLayoutEntry GetDefaultWGPUBindGroupLayoutEntry()
{
    WGPUBindGroupLayoutEntry bindingLayout = {};

    bindingLayout.buffer.nextInChain = nullptr;
    bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
    bindingLayout.buffer.hasDynamicOffset = false;

    bindingLayout.sampler.nextInChain = nullptr;
    bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

    bindingLayout.storageTexture.nextInChain = nullptr;
    bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
    bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
    bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

    bindingLayout.texture.nextInChain = nullptr;
    bindingLayout.texture.multisampled = false;
    bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
    bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;

    return bindingLayout;
}

static WGPUDepthStencilState GetDefaultWebGPUDepthStencilState()
{
    WGPUDepthStencilState depthStencilState = {};
    depthStencilState.nextInChain = nullptr;
    depthStencilState.format = WGPUTextureFormat_Undefined;
    depthStencilState.depthWriteEnabled = false;
    depthStencilState.depthCompare = WGPUCompareFunction_Always;
    depthStencilState.stencilReadMask = 0xFFFFFFFF;
    depthStencilState.stencilWriteMask = 0xFFFFFFFF;
    depthStencilState.depthBias = 0;
    depthStencilState.depthBiasSlopeScale = 0.0;
    depthStencilState.depthBiasClamp = 0.0;
    depthStencilState.stencilFront = {
            .compare = WGPUCompareFunction_Always,
            .failOp = WGPUStencilOperation_Keep,
            .depthFailOp = WGPUStencilOperation_Keep,
            .passOp = WGPUStencilOperation_Keep,
    };
    depthStencilState.stencilBack = {
            .compare = WGPUCompareFunction_Always,
            .failOp = WGPUStencilOperation_Keep,
            .depthFailOp = WGPUStencilOperation_Keep,
            .passOp = WGPUStencilOperation_Keep,
    };
    return depthStencilState;
}

WebGPUShader::WebGPUShader(WGPUDevice device, const std::string &shaderSource, const char *label)
        : m_Device(device), m_ShaderSource(shaderSource)
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

void WebGPUShader::SetVertexBufferLayout(std::vector<WGPUVertexAttribute> attributes, uint64_t stride,
        WGPUVertexStepMode stepMode)
{
    m_VertexAttributes = attributes;
    m_VertexBufferLayout.attributeCount = m_VertexAttributes.size();
    m_VertexBufferLayout.attributes = m_VertexAttributes.data();
    m_VertexBufferLayout.arrayStride = stride; // TODO: stride should be calculated from attributes
    m_VertexBufferLayout.stepMode = stepMode;
}

WGPUVertexState WebGPUShader::GetVertexState()
{
    return {
            .module = m_ShaderModule,
            .entryPoint = m_VertexEntryPoint,
            .constantCount = 0, // TODO: add support for constants
            .constants = nullptr,
            .bufferCount = 1,
            .buffers = &m_VertexBufferLayout
    };
}

WGPUFragmentState WebGPUShader::GetFragmentState()
{
    return {
            .module = m_ShaderModule,
            .entryPoint = m_FragmentEntryPoint,
            .targetCount = m_ColorTargetStates.size(),
            .targets = m_ColorTargetStates.data(),
    };
}

void WebGPUShader::SetColorTargetStates(std::vector<WGPUColorTargetState> colorTargetStates)
{
    m_ColorTargetStates = colorTargetStates;
}
