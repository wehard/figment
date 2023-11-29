#include "WebGPURenderer.h"
#include "WebGPUCommand.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "WebGPUShader.h"
#include "WebGPURenderPipeline.h"
#include "Utils.h"
#include <vector>

#define MEM_ALIGN(_SIZE, _ALIGN)        (((_SIZE) + ((_ALIGN) - 1)) & ~((_ALIGN) - 1))

namespace Figment
{
    RendererStats WebGPURenderer::s_Stats = {};

    WebGPURenderer::WebGPURenderer(WebGPUContext &context)
            : m_Context(context)
    {
        m_IdTexture = new WebGPUTexture(context.GetDevice(), WGPUTextureFormat_R32Sint, context.GetSwapChainWidth(),
                context.GetSwapChainHeight());

        m_PixelBuffer = new WebGPUBuffer<int32_t>(context.GetDevice(), "PixelBuffer", 2048 * 2048 * sizeof(int32_t),
                WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);

        m_DepthTexture = WebGPUTexture::CreateDepthTexture(context.GetDevice(), WGPUTextureFormat_Depth24Plus,
                context.GetSwapChainWidth(), context.GetSwapChainHeight());

        InitShaders();

        m_RendererData.Init();

        auto vertexAttributes = std::vector<WGPUVertexAttribute>({
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
        });

        auto colorTargetStates = std::vector<WGPUColorTargetState>({
                {
                        .format = m_Context.GetTextureFormat(),
                        .blend = nullptr,
                        .writeMask = WGPUColorWriteMask_All
                },
                {
                        .format = m_IdTexture->GetTextureFormat(),
                        .blend = nullptr,
                        .writeMask = WGPUColorWriteMask_All
                }
        });

        m_CircleVertexBuffer = new WebGPUVertexBuffer<CircleVertex>(m_Context.GetDevice(), "CircleVertexBuffer",
                MaxCircleVertexCount * sizeof(CircleVertex));
        m_CircleVertexBuffer->SetVertexLayout(vertexAttributes, sizeof(CircleVertex), WGPUVertexStepMode_Vertex);
        m_QuadVertexBuffer = new WebGPUVertexBuffer<QuadVertex>(m_Context.GetDevice(), "QuadVertexBuffer",
                MaxQuadVertexCount * sizeof(QuadVertex));
        m_QuadVertexBuffer->SetVertexLayout(vertexAttributes, sizeof(QuadVertex), WGPUVertexStepMode_Vertex);
        m_CameraDataUniformBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(),
                "CameraDataUniformBuffer",
                sizeof(CameraData));

        m_QuadPipeline = new WebGPURenderPipeline(m_Context, *m_QuadShader, m_QuadVertexBuffer->GetVertexLayout());
        m_QuadPipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW,
                WGPUCullMode_None);
        m_QuadPipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);
        m_QuadPipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
        m_QuadPipeline->SetColorTargetStates(colorTargetStates);
        m_QuadPipeline->Build();

        m_CirclePipeline = new WebGPURenderPipeline(m_Context, *m_CircleShader,
                m_CircleVertexBuffer->GetVertexLayout());
        m_CirclePipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW,
                WGPUCullMode_None);
        m_CirclePipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);
        m_CirclePipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
        m_CirclePipeline->SetColorTargetStates(colorTargetStates);
        m_CirclePipeline->Build();
    }

    WebGPURenderer::~WebGPURenderer()
    {
        delete m_IdTexture;
        delete m_DepthTexture;
        delete m_CircleVertexBuffer;
        delete m_QuadVertexBuffer;
        delete m_PixelBuffer;
        delete m_CircleShader;
        delete m_QuadShader;
        delete m_CameraDataUniformBuffer;
        delete m_QuadPipeline;
        delete m_CirclePipeline;
    }

    void WebGPURenderer::InitShaders()
    {
        m_CircleShader = new WebGPUShader(m_Context.GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/circle.wgsl"));
        m_QuadShader = new WebGPUShader(m_Context.GetDevice(), *Utils::LoadFile2("res/shaders/wgsl/quad.wgsl"));
    }

    WGPURenderPassEncoder WebGPURenderer::Begin(Figment::Camera &camera)
    {
        CameraData cameraData = {
                .ViewMatrix = camera.GetViewMatrix(),
                .ProjectionMatrix = camera.GetProjectionMatrix()
        };

        m_CameraDataUniformBuffer->SetData(&cameraData, sizeof(cameraData));

        auto device = m_Context.GetDevice();

        // WGPUCommandEncoderDescriptor desc = {};
        // m_CommandEncoder = wgpuDeviceCreateCommandEncoder(device, &desc);
        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(device, "RenderCommandEncoder");

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

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "RenderCommandBuffer");

        WebGPUCommand::SubmitCommandBuffer(m_Context.GetDevice(), commandBuffer);
        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuRenderPassEncoderRelease(m_RenderPass);
        m_CommandEncoder = nullptr;
        m_RenderPass = nullptr;

        s_Stats.CircleCount = m_RendererData.CircleVertexCount / 6;
        s_Stats.QuadCount = m_RendererData.QuadVertexCount / 6;
        s_Stats.VertexCount += m_RendererData.CircleVertexCount + m_RendererData.QuadVertexCount;
    }

    void WebGPURenderer::DrawCircles()
    {
        if (m_RendererData.CircleVertexCount == 0)
            return;

        m_CircleVertexBuffer->SetData(m_RendererData.CircleVertices.data(),
                m_RendererData.CircleVertexCount * sizeof(CircleVertex));

        wgpuRenderPassEncoderSetPipeline(m_RenderPass, m_CirclePipeline->GetPipeline());
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_CircleVertexBuffer->GetBuffer(), 0,
                m_RendererData.CircleVertexCount * sizeof(CircleVertex));
        wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, m_CirclePipeline->GetBindGroup(), 0, nullptr);
        wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.CircleVertexCount, 1, 0, 0);

        s_Stats.DrawCalls++;
    }

    void WebGPURenderer::DrawQuads()
    {
        if (m_RendererData.QuadVertexCount == 0)
            return;

        m_QuadVertexBuffer->SetData(m_RendererData.QuadVertices.data(),
                m_RendererData.QuadVertexCount * sizeof(QuadVertex));



        wgpuRenderPassEncoderSetPipeline(m_RenderPass, m_QuadPipeline->GetPipeline());
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_QuadVertexBuffer->GetBuffer(), 0,
                m_RendererData.QuadVertexCount * sizeof(QuadVertex));
        wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, m_QuadPipeline->GetBindGroup(), 0, nullptr);
        wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.QuadVertexCount, 1, 0, 0);

        s_Stats.DrawCalls++;
    }

    void WebGPURenderer::DrawFigment(FigmentComponent &figment, int32_t id)
    {
        // auto indices = std::vector<uint32_t>();
        // int m = 8;
        // int n = 8;
        // for (int i = 0; i < (m - 1); i++) {
        //     for (int j = 0; j < (n - 1); j++) {
        //         int vertexIndex = i * n + j;
        //
        //         // First triangle forming the quad
        //         indices.push_back(vertexIndex);                        // Top-left vertex
        //         indices.push_back(vertexIndex + 1);                    // Top-right vertex
        //         indices.push_back(vertexIndex + n);                    // Bottom-left vertex
        //
        //         // Second triangle forming the quad
        //         indices.push_back(vertexIndex + n);                    // Bottom-left vertex
        //         indices.push_back(vertexIndex + 1);                    // Top-right vertex
        //         indices.push_back(vertexIndex + n + 1);                // Bottom-right vertex
        //     }
        // }
        // figment.IndexBuffer->SetData(indices.data(), figment.IndexBuffer->GetSize());

        // TODO: Cache pipeline
        auto pipeline = new WebGPURenderPipeline(m_Context, *figment.Shader, figment.VertexBuffer->GetVertexLayout());
        pipeline->SetPrimitiveState(WGPUPrimitiveTopology_PointList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW,
                WGPUCullMode_None);
        pipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat(), WGPUCompareFunction_Less, true);
        pipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
        pipeline->SetBinding(figment.UniformBuffer->GetBindGroupLayoutEntry(1),  figment.UniformBuffer->GetBindGroupEntry(1, 0));
        auto colorTargetStates = std::vector<WGPUColorTargetState>({
                {
                        .format = m_Context.GetTextureFormat(),
                        .blend = nullptr,
                        .writeMask = WGPUColorWriteMask_All
                },
                {
                        .format = m_IdTexture->GetTextureFormat(),
                        .blend = nullptr,
                        .writeMask = WGPUColorWriteMask_All
                }
        });
        pipeline->SetColorTargetStates(colorTargetStates);
        pipeline->Build();


        // wgpuRenderPassEncoderSetIndexBuffer(m_RenderPass, figment.IndexBuffer->GetBuffer(), WGPUIndexFormat_Uint32, 0, figment.IndexBuffer->GetSize());
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, figment.VertexBuffer->GetBuffer(), 0,
                figment.VertexBuffer->GetSize());
        wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipeline->GetPipeline());
        wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, pipeline->GetBindGroup(), 0, nullptr);
        // wgpuRenderPassEncoderDrawIndexed(m_RenderPass, indices.size(), 1, 0, 0, 0);
        wgpuRenderPassEncoderDraw(m_RenderPass, figment.Config.Count, 1, 0, 0);

        s_Stats.VertexCount += figment.VertexBuffer->GetSize() / FigmentComponent::Vertex::Size();
        s_Stats.DrawCalls++;
    }

    void WebGPURenderer::DrawQuad(glm::vec3 position, glm::vec4 color, int32_t id)
    {
        DrawQuad(position, glm::vec3(1.0f), color, id);
    }

    void WebGPURenderer::DrawQuad(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id)
    {
        if (m_RendererData.QuadVertexCount >= MaxQuadVertexCount)
            return;

        for (int i = 0; i < 6; ++i)
        {
            m_RendererData.QuadVertices[m_RendererData.QuadVertexCount + i] = {
                    .WorldPosition = m_QuadVertices[i] * scale + position,
                    .LocalPosition = m_QuadVertices[i] * scale,
                    .Color = color,
                    .Id = id
            };
        }

        m_RendererData.QuadVertexCount += 6;
    }

    void WebGPURenderer::DrawCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id)
    {
        DrawCircle(position, glm::vec3(radius), color, id);
    }

    void WebGPURenderer::DrawCircle(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id)
    {
        if (m_RendererData.CircleVertexCount >= MaxCircleVertexCount)
            return;

        for (int i = 0; i < 6; ++i)
        {
            m_RendererData.CircleVertices[m_RendererData.CircleVertexCount + i] = {
                    .WorldPosition = m_QuadVertices[i] * scale + position,
                    .LocalPosition = m_QuadVertices[i],
                    .Color = color,
                    .Id = id
            };
        }

        m_RendererData.CircleVertexCount += 6;
    }

    void WebGPURenderer::ReadPixel(int x, int y, const std::function<void(int32_t)> &callback)
    {
        m_PixelBuffer->Unmap();
        WebGPUCommand::CopyImageToBuffer(m_Context.GetDevice(), *m_IdTexture, *m_PixelBuffer, 2048, 2048);
        m_PixelBuffer->MapReadAsync([this, callback, x, y](const int32_t *pixels, size_t size)
        {
            auto id = pixels[y * 2048 + x];
            callback(id);
            m_PixelBuffer->Unmap();
        });
    }

    void WebGPURenderer::OnResize(uint32_t width, uint32_t height)
    {
        delete m_IdTexture;
        delete m_DepthTexture;
        m_IdTexture = new WebGPUTexture(m_Context.GetDevice(), WGPUTextureFormat_R32Sint, width, height);
        m_DepthTexture = WebGPUTexture::CreateDepthTexture(m_Context.GetDevice(), WGPUTextureFormat_Depth24Plus,
                width, height);
    }

    void WebGPURenderer::BeginComputePass()
    {
        m_ComputeCommandEncoder = WebGPUCommand::CreateCommandEncoder(m_Context.GetDevice(), "ComputeCommandEncoder");

        WGPUComputePassDescriptor computePassDesc = {};
        computePassDesc.label = "ComputePass";
        computePassDesc.timestampWriteCount = 0;
        computePassDesc.timestampWrites = nullptr;

        m_ComputePass = wgpuCommandEncoderBeginComputePass(m_ComputeCommandEncoder, &computePassDesc);
    }

// void WebGPURenderer::Compute(WebGPUShader &computeShader, WebGPUBuffer<glm::vec4> &buffer,
//         WebGPUBuffer<glm::vec4> &mapBuffer)
    void WebGPURenderer::Compute(Figment::FigmentComponent &figment)
    {
        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = 0;
        bindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
        bindGroupLayoutEntry.buffer.type = WGPUBufferBindingType_Storage;

        WGPUBindGroupLayoutEntry figmentBindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        figmentBindGroupLayoutEntry.nextInChain = nullptr;
        figmentBindGroupLayoutEntry.binding = 1;
        figmentBindGroupLayoutEntry.visibility = WGPUShaderStage_Compute;
        figmentBindGroupLayoutEntry.buffer.type = WGPUBufferBindingType_Uniform;

        std::vector<WGPUBindGroupLayoutEntry> bindGroupLayoutEntries = { bindGroupLayoutEntry,
                                                                         figmentBindGroupLayoutEntry };

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.label = "ComputeBindGroupLayout";
        bindGroupLayoutDesc.entryCount = bindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = bindGroupLayoutEntries.data();

        auto bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Context.GetDevice(), &bindGroupLayoutDesc);

        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.binding = 0;
        bindGroupEntry.buffer = figment.VertexBuffer->GetBuffer();
        bindGroupEntry.offset = 0;
        bindGroupEntry.size = figment.VertexBuffer->GetSize();

        WGPUBindGroupEntry figmentBindGroupEntry = {};
        figmentBindGroupEntry.binding = 1;
        figmentBindGroupEntry.buffer = figment.UniformBuffer->GetBuffer();
        figmentBindGroupEntry.offset = 0;
        figmentBindGroupEntry.size = figment.UniformBuffer->GetSize();

        std::vector<WGPUBindGroupEntry> bindGroupEntries = { bindGroupEntry, figmentBindGroupEntry };
        WGPUBindGroupDescriptor bindGroupDesc = {};
        bindGroupDesc.label = "ComputeBindGroup";
        bindGroupDesc.layout = bindGroupLayout;
        bindGroupDesc.entryCount = bindGroupEntries.size();
        bindGroupDesc.entries = bindGroupEntries.data();

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc = {};
        pipelineLayoutDesc.label = "ComputePipelineLayout";
        pipelineLayoutDesc.bindGroupLayoutCount = 1;
        pipelineLayoutDesc.bindGroupLayouts = &bindGroupLayout;
        auto pipelineLayout = wgpuDeviceCreatePipelineLayout(m_Context.GetDevice(), &pipelineLayoutDesc);

        WGPUComputePipelineDescriptor computePipelineDesc = {};
        computePipelineDesc.label = "ComputePipeline";
        computePipelineDesc.compute.entryPoint = "main";
        computePipelineDesc.compute.module = figment.ComputeShader->GetShaderModule();
        computePipelineDesc.layout = pipelineLayout;

        auto pipeline = wgpuDeviceCreateComputePipeline(m_Context.GetDevice(), &computePipelineDesc);
        auto bindGroup = wgpuDeviceCreateBindGroup(m_Context.GetDevice(), &bindGroupDesc);
        wgpuComputePassEncoderSetPipeline(m_ComputePass, pipeline);
        wgpuComputePassEncoderSetBindGroup(m_ComputePass, 0, bindGroup, 0, nullptr);

        uint32_t invocationCount = figment.VertexBuffer->GetSize() / FigmentComponent::Vertex::Size();
        uint32_t workgroupSize = 32;
        uint32_t workgroupCount = (invocationCount + workgroupSize - 1) / workgroupSize;
        wgpuComputePassEncoderDispatchWorkgroups(m_ComputePass, workgroupCount, 1, 1);

        // figment.MapBuffer->Unmap();
        //
        // WebGPUCommand::CopyBufferToBuffer(m_Context.GetDevice(), *figment.Result, *figment.MapBuffer,
        //         figment.Result->GetSize());
    }

    void WebGPURenderer::EndComputePass()
    {
        wgpuComputePassEncoderEnd(m_ComputePass);

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_ComputeCommandEncoder,
                "ComputeCommandBuffer");
        WebGPUCommand::SubmitCommandBuffer(m_Context.GetDevice(), commandBuffer);

        WebGPUCommand::DestroyCommandEncoder(m_ComputeCommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuComputePassEncoderRelease(m_ComputePass);
        m_ComputeCommandEncoder = nullptr;
        m_ComputePass = nullptr;
    }
}
