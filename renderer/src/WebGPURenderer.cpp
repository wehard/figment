#include "WebGPURenderer.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "WebGPUShader.h"
#include "WebGPURenderPipeline.h"
#include "Utils.h"
#include <utility>
#include <vector>

#define MEM_ALIGN(_SIZE, _ALIGN)        (((_SIZE) + ((_ALIGN) - 1)) & ~((_ALIGN) - 1))

WebGPURenderer::WebGPURenderer(WebGPUContext &context)
        : m_Context(context)
{
    m_Shader = new WebGPUShader(context.GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/default.wgsl"));

    std::vector<float> data = { -0.5, -0.5, 0.0,
                                +0.5, -0.5, 0.0,
                                -0.5, +0.5, 0.0,
                                +0.5, -0.5, 0.0,
                                +0.5, +0.5, 0.0,
                                -0.5, +0.5, 0.0
    };
    m_VertexBuffer = new WebGPUVertexBuffer(context.GetDevice(), data);

    m_IdTexture = new WebGPUTexture(context.GetDevice(), WGPUTextureFormat_R32Sint, context.GetSwapChainWidth(),
            context.GetSwapChainHeight());

    m_PixelBuffer = new WebGPUBuffer<std::byte>(context.GetDevice(), "PixelBuffer", 2048 * 2048 * sizeof(int32_t),
            WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);

    m_DepthTexture = WebGPUTexture::CreateDepthTexture(context.GetDevice(), WGPUTextureFormat_Depth24Plus,
            context.GetSwapChainWidth(), context.GetSwapChainHeight());
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
    colorAttachments[0].clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
    colorAttachments[0].view = wgpuSwapChainGetCurrentTextureView(m_Context.GetSwapChain());

    colorAttachments[1].loadOp = WGPULoadOp_Clear;
    colorAttachments[1].storeOp = WGPUStoreOp_Store;
    colorAttachments[1].clearValue = { -1.0, -1.0, -1.0, -1.0 };
    colorAttachments[1].view = m_IdTexture->GetTextureView();
    colorAttachments[1].resolveTarget = nullptr;

    WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
    depthStencilAttachment.view = m_DepthTexture->GetTextureView();
    depthStencilAttachment.depthClearValue = 1.0f;
    depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
    depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
    depthStencilAttachment.depthReadOnly = false;
    depthStencilAttachment.stencilClearValue = 0;
    depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
    depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
    depthStencilAttachment.stencilReadOnly = true;

    WGPURenderPassDescriptor renderPassDesc = {};
    renderPassDesc.colorAttachmentCount = 2;
    renderPassDesc.colorAttachments = colorAttachments;
    renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

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

void WebGPURenderer::DrawQuad(glm::mat4 transform, glm::vec4 color, int32_t id)
{
    RenderData renderData = {};
    renderData.Color = color;
    renderData.ModelMatrix = transform;
    renderData.ViewMatrix = m_RenderPassData.ViewMatrix;
    renderData.ProjectionMatrix = m_RenderPassData.ProjectionMatrix;
    renderData.Id = id;
    auto uniformRenderData = new WebGPUUniformBuffer<RenderData>(m_Context.GetDevice(), &renderData,
            sizeof(renderData));

    auto pipelineBuilder = WebGPURenderPipelineBuilder(m_Context, *m_Shader);
    pipelineBuilder.SetVertexBufferLayout({
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0,
            }}, 3 * sizeof(float), WGPUVertexStepMode_Vertex);

    pipelineBuilder.AddColorTargetState(0, m_Context.GetTextureFormat(), WGPUColorWriteMask_All, false);
    pipelineBuilder.AddColorTargetState(1, m_IdTexture->GetTextureFormat(), WGPUColorWriteMask_All, true);
    pipelineBuilder.SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined, WGPUFrontFace_CCW, WGPUCullMode_None);

    pipelineBuilder.SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);

    pipelineBuilder.AddBinding(0, uniformRenderData->GetBuffer(), 0, uniformRenderData->GetSize());
    pipelineBuilder.AddBindGroupLayoutEntry(0, WGPUBufferBindingType_Uniform, WGPUShaderStage_Vertex | WGPUShaderStage_Fragment, uniformRenderData->GetSize());

    pipelineBuilder.Create();

    wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipelineBuilder.GetPipeline());
    wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_VertexBuffer->m_Buffer, 0, m_VertexBuffer->m_Size);
    wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, pipelineBuilder.GetBindGroup(), 0, nullptr);
    wgpuRenderPassEncoderDraw(m_RenderPass, m_VertexBuffer->m_VertexCount, 1, 0, 0);
}

void WebGPURenderer::ReadPixel(int x, int y, std::function<void(int32_t)> callback)
{
    auto mapState = m_PixelBuffer->GetMapState();
    if (mapState == WGPUBufferMapState_Mapped)
    {
        printf("Buffer is already mapped\n");
        return;
    }

    m_PixelBuffer->Unmap();

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
    imageCopyBuffer.buffer = m_PixelBuffer->GetBuffer();
    imageCopyBuffer.layout.offset = 0;
    imageCopyBuffer.layout.bytesPerRow = 2048 * sizeof(int32_t);
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
        WGPUBuffer buffer = nullptr;
        uint32_t x = 0;
        uint32_t y = 0;
        std::function<void(int32_t)> callback;
    };

    auto *callbackData = new BufferMapReadCallbackData();
    callbackData->buffer = m_PixelBuffer->GetBuffer();
    callbackData->x = x;
    callbackData->y = y;
    callbackData->callback = std::move(callback);

    wgpuBufferMapAsync(m_PixelBuffer->GetBuffer(), WGPUMapMode_Read, 0, 2048 * 2048 * sizeof(int32_t),
            [](WGPUBufferMapAsyncStatus status, void *userData)
            {
                auto *callbackData = (BufferMapReadCallbackData *)userData;
                if (status != WGPUBufferMapAsyncStatus_Success)
                {
                    printf("Buffer map failed with WGPUBufferMapAsyncStatus: %d\n", status);
                    return;
                }
                auto *pixels = (int32_t *)wgpuBufferGetConstMappedRange(callbackData->buffer, 0,
                        2048 * 2048 * sizeof(int32_t));
                auto id = pixels[callbackData->y * 2048 + callbackData->x];
                wgpuBufferUnmap(callbackData->buffer);
                callbackData->callback(id);
                delete callbackData;
            }, (void *)callbackData);
}

void WebGPURenderer::OnResize(uint32_t width, uint32_t height)
{
    delete m_IdTexture;
    delete m_DepthTexture;
    m_IdTexture = new WebGPUTexture(m_Context.GetDevice(), WGPUTextureFormat_R32Sint, width, height);
    m_DepthTexture = WebGPUTexture::CreateDepthTexture(m_Context.GetDevice(), WGPUTextureFormat_Depth24Plus,
            width, height);
}
