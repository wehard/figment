#include "WebGPURenderer.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include <vector>

static std::string *loadShaderFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        printf("Failed to open file: %s\n", filename);
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    auto *result = new std::string(size, '\0');
    fread(result->data(), 1, size, file);
    fclose(file);
    return result;
}

static WGPUShaderModule CreateShaderModule(WGPUDevice device, const std::string &shaderSource)
{
    WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {};
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = shaderSource.c_str();

    WGPUShaderModuleDescriptor shaderDesc = {};
    shaderDesc.nextInChain = &shaderCodeDesc.chain;

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);
    return shaderModule;
}

WebGPURenderer::WebGPURenderer(WebGPUContext &context)
        : m_Context(context)
{
    m_ShaderModule = CreateShaderModule(context.GetDevice(), *loadShaderFile("res/shaders/wgsl/default.wgsl"));

    std::vector<float> data = { -0.5, -0.5, 0.0,
                                +0.5, -0.5, 0.0,
                                -0.5, +0.5, 0.0,
                                +0.5, -0.5, 0.0,
                                -0.5, +0.5, 0.0,
                                +0.5, +0.5, 0.0
    };
    m_VertexBuffer = new WebGPUVertexBuffer(context.GetDevice(), data);

    m_IdTexture = new WebGPUTexture(context.GetDevice(), WGPUTextureFormat_R32Uint, context.GetSwapChainWidth(),
            context.GetSwapChainHeight());

    WGPUBufferDescriptor pixelBufferDesc = {};
    pixelBufferDesc.nextInChain = nullptr;
    pixelBufferDesc.label = "PixelBuffer";
    pixelBufferDesc.mappedAtCreation = false;
    pixelBufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
    pixelBufferDesc.size = 2048 * 2048 * sizeof(uint32_t);
    m_PixelBuffer = wgpuDeviceCreateBuffer(context.GetDevice(), &pixelBufferDesc);
}

WGPURenderPassEncoder WebGPURenderer::Begin(Camera &camera)
{
    m_RenderPassData.ProjectionMatrix = camera.GetProjectionMatrix();
    m_RenderPassData.ViewMatrix = camera.GetViewMatrix();

    auto device = m_Context.GetDevice();

    WGPUCommandEncoderDescriptor desc = {};
    m_CommandEncoder = wgpuDeviceCreateCommandEncoder(device, &desc);

    WGPURenderPassColorAttachment colorAttachments[2] = {};

    colorAttachments[0].loadOp = WGPULoadOp_Clear;
    colorAttachments[0].storeOp = WGPUStoreOp_Store;
    colorAttachments[0].clearValue = { 0.1, 0.1, 0.1, 1 };
    colorAttachments[0].view = wgpuSwapChainGetCurrentTextureView(m_Context.GetSwapChain());

    colorAttachments[1].loadOp = WGPULoadOp_Clear;
    colorAttachments[1].storeOp = WGPUStoreOp_Store;
    colorAttachments[1].clearValue = { 42.0, 0.0, 0.0, 0.0 };
    colorAttachments[1].view = m_IdTexture->GetTextureView();
    colorAttachments[1].resolveTarget = nullptr;

    WGPURenderPassDescriptor renderPassDesc = {};
    renderPassDesc.colorAttachmentCount = 2;
    renderPassDesc.colorAttachments = colorAttachments;
    renderPassDesc.depthStencilAttachment = nullptr;

    m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);
    return m_RenderPass;
}

void WebGPURenderer::End()
{
    wgpuRenderPassEncoderEnd(m_RenderPass);

    WGPUCommandBufferDescriptor commandBufferDesc = {};
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(m_CommandEncoder, &commandBufferDesc);
    WGPUQueue queue = wgpuDeviceGetQueue(m_Context.GetDevice());
    wgpuQueueSubmit(queue, 1, &commandBuffer);

    wgpuCommandEncoderRelease(m_CommandEncoder);
    m_CommandEncoder = nullptr;
    wgpuRenderPassEncoderRelease(m_RenderPass);
    m_RenderPass = nullptr;
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

void WebGPURenderer::DrawQuad(glm::mat4 transform, glm::vec4 color)
{
    RenderData renderData = {};
    renderData.Color = color;
    renderData.ModelMatrix = transform;
    renderData.ViewMatrix = m_RenderPassData.ViewMatrix;
    renderData.ProjectionMatrix = m_RenderPassData.ProjectionMatrix;
    auto uniformRenderData = new WebGPUUniformBuffer<RenderData>(m_Context.GetDevice(), &renderData,
            sizeof(renderData));

    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.nextInChain = nullptr;

    WGPUVertexBufferLayout vertexBufferLayout = {};

    WGPUVertexAttribute vertexAttrib = {};
    vertexAttrib.shaderLocation = 0;
    vertexAttrib.format = WGPUVertexFormat_Float32x3;
    vertexAttrib.offset = 0;

    vertexBufferLayout.attributeCount = 1;
    vertexBufferLayout.attributes = &vertexAttrib;
    vertexBufferLayout.arrayStride = 3 * sizeof(float);
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;

    pipelineDesc.vertex.module = m_ShaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;

    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;

    WGPUBlendState blendState = {};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTargets[2] = {};

    colorTargets[0].nextInChain = nullptr;
    colorTargets[0].format = m_Context.GetTextureFormat();
    colorTargets[0].blend = &blendState;
    colorTargets[0].writeMask = WGPUColorWriteMask_All;

    colorTargets[1].nextInChain = nullptr;
    colorTargets[1].format = m_IdTexture->GetTextureFormat();
    colorTargets[1].blend = nullptr;
    colorTargets[1].writeMask = WGPUColorWriteMask_None;

    WGPUFragmentState fragmentState = {};
    fragmentState.module = m_ShaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    fragmentState.targetCount = 2;
    fragmentState.targets = colorTargets;

    pipelineDesc.fragment = &fragmentState;
    pipelineDesc.depthStencil = nullptr;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    WGPUBindGroupLayoutEntry bindingLayout = GetDefaultWGPUBindGroupLayoutEntry();
    bindingLayout.binding = 0;
    bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayout.buffer.minBindingSize = uniformRenderData->m_Size;

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.nextInChain = nullptr;
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindingLayout;
    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Context.GetDevice(), &bindGroupLayoutDesc);

    WGPUPipelineLayoutDescriptor layoutDesc = {};
    layoutDesc.nextInChain = nullptr;
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &bindGroupLayout;

    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(m_Context.GetDevice(), &layoutDesc);

    pipelineDesc.layout = layout;

    WGPUBindGroupEntry binding = {};
    binding.nextInChain = nullptr;
    binding.binding = 0;
    binding.buffer = uniformRenderData->m_Buffer;
    binding.offset = 0;
    binding.size = uniformRenderData->m_Size;

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = bindGroupLayout;
    bindGroupDesc.entryCount = bindGroupLayoutDesc.entryCount;
    bindGroupDesc.entries = &binding;
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(m_Context.GetDevice(), &bindGroupDesc);

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &pipelineDesc);

    wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipeline);

    wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_VertexBuffer->m_Buffer, 0, m_VertexBuffer->m_Size);

    wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, bindGroup, 0, nullptr);

    wgpuRenderPassEncoderDraw(m_RenderPass, m_VertexBuffer->m_VertexCount, 1, 0, 0);
}

uint32_t WebGPURenderer::ReadPixel(int x, int y)
{
    WGPUCommandEncoderDescriptor commandEncoderDesc = {};
    commandEncoderDesc.nextInChain = nullptr;
    commandEncoderDesc.label = "ReadPixelCommandEncoder";
    WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(m_Context.GetDevice(), &commandEncoderDesc);

    WGPUImageCopyTexture imageCopyTexture = {};
    imageCopyTexture.nextInChain = nullptr;
    imageCopyTexture.origin = { 0, 0, 0 };
    imageCopyTexture.texture = m_IdTexture->GetTexture();

    WGPUImageCopyBuffer imageCopyBuffer = {};
    imageCopyBuffer.nextInChain = nullptr;
    imageCopyBuffer.buffer = m_PixelBuffer;
    imageCopyBuffer.layout.offset = 0;
    imageCopyBuffer.layout.bytesPerRow = 2048 * sizeof(uint32_t);
    imageCopyBuffer.layout.rowsPerImage = 2048;

    WGPUExtent3D copySize = {};
    copySize.width = m_IdTexture->GetWidth();
    copySize.height = m_IdTexture->GetHeight();
    copySize.depthOrArrayLayers = 1;
    wgpuCommandEncoderCopyTextureToBuffer(commandEncoder, &imageCopyTexture, &imageCopyBuffer, &copySize);

    auto commandBuffer = wgpuCommandEncoderFinish(commandEncoder, nullptr);
    WGPUQueue queue = wgpuDeviceGetQueue(m_Context.GetDevice());
    wgpuQueueSubmit(queue, 1, &commandBuffer);

    struct BufferMapReadCallbackData
    {
        WGPUBuffer buffer;
        uint32_t x;
        uint32_t y;
    };

    auto *callbackData = new BufferMapReadCallbackData();
    callbackData->buffer = m_PixelBuffer;
    callbackData->x = x;
    callbackData->y = y;

    wgpuBufferMapAsync(m_PixelBuffer, WGPUMapMode_Read, 0, 2048 * 2048 * sizeof(uint32_t),
            [](WGPUBufferMapAsyncStatus status, void *userData)
            {
                auto *callbackData = (BufferMapReadCallbackData *)userData;
                if (status != WGPUBufferMapAsyncStatus_Success)
                {
                    printf("Buffer map failed with WGPUBufferMapAsyncStatus: %d\n", status);
                    return;
                }
                auto *pixels = (uint32_t *)wgpuBufferGetConstMappedRange(callbackData->buffer, 0,
                        2048 * 2048 * sizeof(uint32_t));
                printf("Pixel: %d\n", pixels[callbackData->y * 2048 + callbackData->x]);
                wgpuBufferUnmap(callbackData->buffer);
                delete callbackData;
            }, (void *)callbackData);

    return 0;
}