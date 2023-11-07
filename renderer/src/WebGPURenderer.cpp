#include "WebGPURenderer.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "WebGPUShader.h"
#include "WebGPURenderPipeline.h"
#include "Utils.h"
#include <utility>
#include <vector>

#define MEM_ALIGN(_SIZE, _ALIGN)        (((_SIZE) + ((_ALIGN) - 1)) & ~((_ALIGN) - 1))

RendererStats WebGPURenderer::s_Stats = {};

WebGPURenderer::WebGPURenderer(WebGPUContext &context)
        : m_Context(context)
{
    m_Shader = new WebGPUShader(context.GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/default.wgsl"));
    m_CircleShader = new WebGPUShader(context.GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/circle.wgsl"));
    m_QuadShader = new WebGPUShader(context.GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/quad.wgsl"));

    std::vector<float> data = { -0.5, -0.5, 0.0,
                                +0.5, -0.5, 0.0,
                                -0.5, +0.5, 0.0,
                                +0.5, -0.5, 0.0,
                                +0.5, +0.5, 0.0,
                                -0.5, +0.5, 0.0
    };

    m_IdTexture = new WebGPUTexture(context.GetDevice(), WGPUTextureFormat_R32Sint, context.GetSwapChainWidth(),
            context.GetSwapChainHeight());

    m_PixelBuffer = new WebGPUBuffer<std::byte>(context.GetDevice(), "PixelBuffer", 2048 * 2048 * sizeof(int32_t),
            WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);

    m_DepthTexture = WebGPUTexture::CreateDepthTexture(context.GetDevice(), WGPUTextureFormat_Depth24Plus,
            context.GetSwapChainWidth(), context.GetSwapChainHeight());

    m_RendererData.Init();

    m_CircleVertexBuffer = new WebGPUBuffer<CircleVertex>(m_Context.GetDevice(), "CircleVertexBuffer",
            MaxCircleVertexCount * sizeof(CircleVertex), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
    m_QuadVertexBuffer = new WebGPUBuffer<QuadVertex>(m_Context.GetDevice(), "QuadVertexBuffer",
            MaxQuadVertexCount * sizeof(QuadVertex), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
    m_CameraDataUniformBuffer = new WebGPUBuffer<CameraData>(m_Context.GetDevice(), "CameraDataUniformBuffer",
            sizeof(CameraData), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
}

WebGPURenderer::~WebGPURenderer()
{
    delete m_IdTexture;
    delete m_DepthTexture;
    delete m_CircleVertexBuffer;
    delete m_QuadVertexBuffer;
    delete m_PixelBuffer;
    delete m_Shader;
    delete m_CircleShader;
    delete m_QuadShader;
    delete m_CameraDataUniformBuffer;
}

WGPURenderPassEncoder WebGPURenderer::Begin(Camera &camera)
{
    CameraData cameraData = {
            .ViewMatrix = camera.GetViewMatrix(),
            .ProjectionMatrix = camera.GetProjectionMatrix()
    };

    m_CameraDataUniformBuffer->SetData(&cameraData, sizeof(cameraData));

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

    m_RendererData.Reset();
    s_Stats.Reset();

    return m_RenderPass;
}

void WebGPURenderer::End()
{
    DrawCircles();
    DrawQuads();

    wgpuRenderPassEncoderEnd(m_RenderPass);

    WGPUCommandBufferDescriptor commandBufferDesc = {};
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(m_CommandEncoder, &commandBufferDesc);
    WGPUQueue queue = wgpuDeviceGetQueue(m_Context.GetDevice());
    wgpuQueueSubmit(queue, 1, &commandBuffer);

    wgpuCommandEncoderRelease(m_CommandEncoder);
    m_CommandEncoder = nullptr;
    wgpuRenderPassEncoderRelease(m_RenderPass);
    m_RenderPass = nullptr;

    s_Stats.CircleCount = m_RendererData.CircleVertexCount / 6;
    s_Stats.QuadCount = m_RendererData.QuadVertexCount / 6;
    s_Stats.VertexCount = m_RendererData.CircleVertexCount + m_RendererData.QuadVertexCount;
}

void WebGPURenderer::DrawCircles()
{
    if (m_RendererData.CircleVertexCount == 0)
        return;

    // auto cameraDataBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(), &m_CameraData,
    //         sizeof(m_CameraData));

    m_CircleVertexBuffer->SetData(m_RendererData.CircleVertices.data(), m_RendererData.CircleVertexCount * sizeof(CircleVertex));

    auto pipelineBuilder = WebGPURenderPipelineBuilder(m_Context, *m_CircleShader);
    pipelineBuilder.SetVertexBufferLayout({
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0,
            },
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 3 * sizeof(float),
                    .shaderLocation = 1,
            },
            {
                    .format = WGPUVertexFormat_Float32x4,
                    .offset = 6 * sizeof(float),
                    .shaderLocation = 2,
            },
            {
                    .format = WGPUVertexFormat_Sint32,
                    .offset = 10 * sizeof(float),
                    .shaderLocation = 3,
            }
    }, sizeof(CircleVertex), WGPUVertexStepMode_Vertex);

    pipelineBuilder.AddColorTargetState(0, m_Context.GetTextureFormat(), WGPUColorWriteMask_All, false);
    pipelineBuilder.AddColorTargetState(1, m_IdTexture->GetTextureFormat(), WGPUColorWriteMask_All, true);
    pipelineBuilder.SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined, WGPUFrontFace_CCW,
            WGPUCullMode_None);

    pipelineBuilder.SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);

    pipelineBuilder.AddBinding(0, m_CameraDataUniformBuffer->GetBuffer(), 0, m_CameraDataUniformBuffer->GetSize());
    pipelineBuilder.AddBindGroupLayoutEntry(0, WGPUBufferBindingType_Uniform,
            WGPUShaderStage_Vertex | WGPUShaderStage_Fragment, m_CameraDataUniformBuffer->GetSize());

    pipelineBuilder.Build();

    wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipelineBuilder.GetPipeline());
    wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_CircleVertexBuffer->GetBuffer(), 0,
            m_RendererData.CircleVertexCount * sizeof(CircleVertex));
    wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, pipelineBuilder.GetBindGroup(), 0, nullptr);
    wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.CircleVertexCount, 1, 0, 0);

    s_Stats.DrawCalls++;
}

void WebGPURenderer::DrawQuads()
{
    if (m_RendererData.QuadVertexCount == 0)
        return;

    m_QuadVertexBuffer->SetData(m_RendererData.QuadVertices.data(), m_RendererData.QuadVertexCount * sizeof(QuadVertex));

    auto pipelineBuilder = WebGPURenderPipelineBuilder(m_Context, *m_QuadShader);
    pipelineBuilder.SetVertexBufferLayout({
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 0,
                    .shaderLocation = 0,
            },
            {
                    .format = WGPUVertexFormat_Float32x3,
                    .offset = 3 * sizeof(float),
                    .shaderLocation = 1,
            },
            {
                    .format = WGPUVertexFormat_Float32x4,
                    .offset = 6 * sizeof(float),
                    .shaderLocation = 2,
            },
            {
                    .format = WGPUVertexFormat_Sint32,
                    .offset = 10 * sizeof(float),
                    .shaderLocation = 3,
            }
    }, sizeof(CircleVertex), WGPUVertexStepMode_Vertex);

    pipelineBuilder.AddColorTargetState(0, m_Context.GetTextureFormat(), WGPUColorWriteMask_All, false);
    pipelineBuilder.AddColorTargetState(1, m_IdTexture->GetTextureFormat(), WGPUColorWriteMask_All, true);
    pipelineBuilder.SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined, WGPUFrontFace_CCW,
            WGPUCullMode_None);

    pipelineBuilder.SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);

    pipelineBuilder.AddBinding(0, m_CameraDataUniformBuffer->GetBuffer(), 0, m_CameraDataUniformBuffer->GetSize());
    pipelineBuilder.AddBindGroupLayoutEntry(0, WGPUBufferBindingType_Uniform,
            WGPUShaderStage_Vertex | WGPUShaderStage_Fragment, m_CameraDataUniformBuffer->GetSize());

    pipelineBuilder.Build();

    wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipelineBuilder.GetPipeline());
    wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_QuadVertexBuffer->GetBuffer(), 0,
            m_RendererData.QuadVertexCount * sizeof(QuadVertex));
    wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, pipelineBuilder.GetBindGroup(), 0, nullptr);
    wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.QuadVertexCount, 1, 0, 0);

    s_Stats.DrawCalls++;
}

void WebGPURenderer::DrawQuad(glm::vec3 position, glm::vec4 color, int32_t id)
{
    if (m_RendererData.QuadVertexCount >= MaxQuadVertexCount - 6)
        return;

    for (int i = 0; i < 6; ++i)
    {
        m_RendererData.QuadVertices[m_RendererData.QuadVertexCount + i] = {
                .WorldPosition = m_QuadVertices[i] + position,
                .LocalPosition = m_QuadVertices[i],
                .Color = color,
                .Id = id
        };
    }

    m_RendererData.QuadVertexCount += 6;
}

void WebGPURenderer::DrawCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id)
{
    if (m_RendererData.CircleVertexCount >= MaxCircleVertexCount - 6)
        return;

    for (int i = 0; i < 6; ++i)
    {
        m_RendererData.CircleVertices[m_RendererData.CircleVertexCount + i] = {
                .WorldPosition = m_QuadVertices[i] * radius + position,
                .LocalPosition = m_QuadVertices[i],
                .Color = color,
                .Id = id
        };
    }

    m_RendererData.CircleVertexCount += 6;
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

