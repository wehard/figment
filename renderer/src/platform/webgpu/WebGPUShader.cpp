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

    // wgpuShaderModuleGetCompilationInfo(m_ShaderModule, CompilationInfoCallback, nullptr);
}

WebGPUShader::~WebGPUShader()
{
    wgpuShaderModuleRelease(m_ShaderModule);
}

WGPUVertexState WebGPUShader::GetVertexStage()
{
    m_VertexAttribute.shaderLocation = 0;
    m_VertexAttribute.format = WGPUVertexFormat_Float32x3;
    m_VertexAttribute.offset = 0;

    m_VertexBufferLayout.attributeCount = 1;
    m_VertexBufferLayout.attributes = &m_VertexAttribute;
    m_VertexBufferLayout.arrayStride = 3 * sizeof(float);
    m_VertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    WGPUVertexState vertexState = {};
    vertexState.bufferCount = 1;
    vertexState.buffers = &m_VertexBufferLayout;

    vertexState.module = m_ShaderModule;
    vertexState.entryPoint = "vs_main";
    vertexState.constantCount = 0;
    vertexState.constants = nullptr;

    return vertexState;
}

WGPUFragmentState WebGPUShader::GetFragmentStage(WGPUTextureFormat textureFormat, WGPUTextureFormat idFormat)
{
    m_BlendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    m_BlendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    m_BlendState.color.operation = WGPUBlendOperation_Add;

    m_BlendState.alpha.srcFactor = WGPUBlendFactor_One;
    m_BlendState.alpha.dstFactor = WGPUBlendFactor_One;
    m_BlendState.alpha.operation = WGPUBlendOperation_Add;

    m_ColorTargets[0].nextInChain = nullptr;
    m_ColorTargets[0].format = textureFormat;
    m_ColorTargets[0].blend = &m_BlendState;
    m_ColorTargets[0].writeMask = WGPUColorWriteMask_All;

    m_ColorTargets[1].nextInChain = nullptr;
    m_ColorTargets[1].format = idFormat;
    m_ColorTargets[1].blend = nullptr;
    m_ColorTargets[1].writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState = {};
    fragmentState.module = m_ShaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    fragmentState.targetCount = 2;
    fragmentState.targets = m_ColorTargets;

    return fragmentState;
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

WGPUPipelineLayout WebGPUShader::GetPipelineLayout(uint64_t uniformSize)
{
    WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
    bindGroupLayoutEntry.binding = 0;
    bindGroupLayoutEntry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bindGroupLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
    bindGroupLayoutEntry.buffer.minBindingSize = uniformSize;

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.nextInChain = nullptr;
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Device, &bindGroupLayoutDesc);

    WGPUPipelineLayoutDescriptor layoutDesc = {};
    layoutDesc.nextInChain = nullptr;
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &bindGroupLayout;
    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(m_Device, &layoutDesc);

    return layout;
}

static WGPUDepthStencilState GetDefaultDepthStencilState()
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

const WGPUDepthStencilState WebGPUShader::GetDepthStencilState(WGPUTextureFormat depthFormat)
{
    m_DepthStencilState = GetDefaultDepthStencilState();
    m_DepthStencilState.depthCompare = WGPUCompareFunction_Less;
    m_DepthStencilState.depthWriteEnabled = true;
    m_DepthStencilState.format = depthFormat;
    m_DepthStencilState.stencilReadMask = 0;
    m_DepthStencilState.stencilWriteMask = 0;
    return m_DepthStencilState;
}
