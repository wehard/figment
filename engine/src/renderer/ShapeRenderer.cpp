#include "ShapeRenderer.h"
#include "WebGPUCommand.h"
#include "WebGPUBuffer.h"
#include "WebGPUTexture.h"
#include "WebGPUShader.h"
#include "WebGPURenderPipeline.h"
#include "FileUtils.h"
#include "RenderPass.h"
#include "RenderStats.h"
#include <vector>

namespace Figment
{
    ShapeRenderer::ShapeRenderer(WebGPUContext &context)
            : m_Context(context), m_RenderTarget(context.GetDefaultRenderTarget())
    {
        m_IdTexture = new WebGPUTexture(context.GetDevice(), {
                .Format = WGPUTextureFormat_R32Sint,
                .Width = context.GetSwapChainWidth(),
                .Height = context.GetSwapChainHeight(),
                .Usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc,
        });

        m_PixelBuffer = new WebGPUBuffer<int32_t>(context.GetDevice(), "PixelBuffer", 2048 * 2048 * sizeof(int32_t),
                WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);

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
                        .format = m_RenderTarget->Color.TextureFormat,
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
        m_LineVertexBuffer = new WebGPUVertexBuffer<LineVertex>(m_Context.GetDevice(), "LineVertexBuffer",
                MaxLineVertexCount * sizeof(LineVertex));
        m_LineVertexBuffer->SetVertexLayout(vertexAttributes, sizeof(LineVertex), WGPUVertexStepMode_Vertex);

        m_CameraDataUniformBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(),
                "CameraDataUniformBuffer",
                sizeof(CameraData));

        m_QuadPipeline = new WebGPURenderPipeline(m_Context.GetDevice(), *m_QuadShader,
                m_QuadVertexBuffer->GetVertexLayout());
        m_QuadPipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW,
                WGPUCullMode_None);
        m_QuadPipeline->SetDepthStencilState(m_RenderTarget->Depth.TextureFormat, WGPUCompareFunction_Less, true);
        m_QuadPipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
        m_QuadPipeline->SetColorTargetStates(colorTargetStates);
        m_QuadPipeline->Build();

        m_CirclePipeline = new WebGPURenderPipeline(m_Context.GetDevice(), *m_CircleShader,
                m_CircleVertexBuffer->GetVertexLayout());
        m_CirclePipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW,
                WGPUCullMode_None);
        m_CirclePipeline->SetDepthStencilState(m_RenderTarget->Depth.TextureFormat, WGPUCompareFunction_Less, true);
        m_CirclePipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
        m_CirclePipeline->SetColorTargetStates(colorTargetStates);
        m_CirclePipeline->Build();

        m_LinePipeline = new WebGPURenderPipeline(m_Context.GetDevice(), *m_LineShader,
                m_LineVertexBuffer->GetVertexLayout());
        m_LinePipeline->SetPrimitiveState(WGPUPrimitiveTopology_LineList, WGPUIndexFormat_Undefined, WGPUFrontFace_CCW,
                WGPUCullMode_None);
        m_LinePipeline->SetDepthStencilState(m_RenderTarget->Depth.TextureFormat, WGPUCompareFunction_Less, true);
        m_LinePipeline->SetBinding(m_CameraDataUniformBuffer->GetBindGroupLayoutEntry(0),
                m_CameraDataUniformBuffer->GetBindGroupEntry(0, 0));
        m_LinePipeline->SetColorTargetStates(colorTargetStates);
        m_LinePipeline->Build();

        m_GridUniformBuffer = new WebGPUUniformBuffer<GridData>(m_Context.GetDevice(), "GridUniformBuffer",
                sizeof(GridData));
    }

    ShapeRenderer::~ShapeRenderer()
    {
        delete m_IdTexture;
        delete m_CircleVertexBuffer;
        delete m_QuadVertexBuffer;
        delete m_PixelBuffer;
        delete m_CircleShader;
        delete m_QuadShader;
        delete m_CameraDataUniformBuffer;
        delete m_QuadPipeline;
        delete m_CirclePipeline;
        delete m_GridUniformBuffer;
    }

    void ShapeRenderer::InitShaders()
    {
        m_CircleShader = new WebGPUShader(m_Context.GetDevice(), "res/shaders/builtin/circle.wgsl",
                "Circle");
        m_QuadShader = new WebGPUShader(m_Context.GetDevice(), "res/shaders/builtin/quad.wgsl",
                "Quad");
        m_LineShader = new WebGPUShader(m_Context.GetDevice(), "res/shaders/builtin/line.wgsl", "Line");
    }

    void ShapeRenderer::Begin(Figment::Camera &camera)
    {
        m_CameraData = {
                .ViewMatrix = camera.GetViewMatrix(),
                .ProjectionMatrix = camera.GetProjectionMatrix()
        };

        m_CameraDataUniformBuffer->SetData(&m_CameraData, sizeof(m_CameraData));

        auto colorAttachment = m_RenderTarget->GetColorAttachment();
        auto depthStencilAttachment = m_RenderTarget->GetDepthStencilAttachment();

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;
        renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(m_Context.GetDevice(), "RenderCommandEncoder");
        m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);

        m_RendererData.Reset();
    }

    void ShapeRenderer::End()
    {
        DrawCircles();
        DrawQuads();
        DrawLines();

        wgpuRenderPassEncoderEnd(m_RenderPass);

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "RenderCommandBuffer");

        WebGPUCommand::SubmitCommandBuffer(m_Context.GetDevice(), commandBuffer);
        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuRenderPassEncoderRelease(m_RenderPass);
        m_CommandEncoder = nullptr;
        m_RenderPass = nullptr;

        RenderStats::CircleCount = m_RendererData.CircleVertexCount / 6;
        RenderStats::QuadCount = m_RendererData.QuadVertexCount / 6;
        RenderStats::VertexCount += m_RendererData.CircleVertexCount + m_RendererData.QuadVertexCount;
    }

    void ShapeRenderer::DrawCircles()
    {
        if (m_RendererData.CircleVertexCount == 0)
            return;

        m_CircleVertexBuffer->SetData(m_RendererData.CircleVertices.data(),
                m_RendererData.CircleVertexCount * sizeof(CircleVertex));

        wgpuRenderPassEncoderSetPipeline(m_RenderPass, m_CirclePipeline->Pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_CircleVertexBuffer->GetBuffer(), 0,
                m_RendererData.CircleVertexCount * sizeof(CircleVertex));
        wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, m_CirclePipeline->BindGroup, 0, nullptr);
        wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.CircleVertexCount, 1, 0, 0);

        RenderStats::DrawCalls++;
    }

    void ShapeRenderer::DrawQuads()
    {
        if (m_RendererData.QuadVertexCount == 0)
            return;

        m_QuadVertexBuffer->SetData(m_RendererData.QuadVertices.data(),
                m_RendererData.QuadVertexCount * sizeof(QuadVertex));

        wgpuRenderPassEncoderSetPipeline(m_RenderPass, m_QuadPipeline->Pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_QuadVertexBuffer->GetBuffer(), 0,
                m_RendererData.QuadVertexCount * sizeof(QuadVertex));
        wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, m_QuadPipeline->BindGroup, 0, nullptr);
        wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.QuadVertexCount, 1, 0, 0);

        RenderStats::DrawCalls++;
    }

    void ShapeRenderer::DrawLines()
    {
        if (m_RendererData.LineVertexCount == 0)
            return;

        m_LineVertexBuffer->SetData(m_RendererData.LineVertices.data(),
                m_RendererData.LineVertexCount * sizeof(LineVertex));

        wgpuRenderPassEncoderSetPipeline(m_RenderPass, m_LinePipeline->Pipeline);
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_LineVertexBuffer->GetBuffer(), 0,
                m_RendererData.LineVertexCount * sizeof(LineVertex));
        wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, m_LinePipeline->BindGroup, 0, nullptr);
        wgpuRenderPassEncoderDraw(m_RenderPass, m_RendererData.LineVertexCount, 1, 0, 0);

        RenderStats::DrawCalls++;
    }

    void ShapeRenderer::SubmitQuad(glm::vec3 position, glm::vec4 color, int32_t id)
    {
        SubmitQuad(position, glm::vec3(1.0f), color, id);
    }

    void ShapeRenderer::SubmitQuad(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id)
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

    void ShapeRenderer::SubmitCircle(glm::vec3 position, glm::vec4 color, float radius, int32_t id)
    {
        SubmitCircle(position, glm::vec3(radius), color, id);
    }

    void ShapeRenderer::SubmitCircle(glm::vec3 position, glm::vec3 scale, glm::vec4 color, int32_t id)
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

    void ShapeRenderer::SubmitLine(glm::vec3 start, glm::vec3 end, glm::vec4 color, int32_t id)
    {
        if (m_RendererData.LineVertexCount >= MaxLineVertexCount)
            return;

        m_RendererData.LineVertices[m_RendererData.LineVertexCount] = {
                .WorldPosition = start,
                .LocalPosition = start,
                .Color = color,
                .Id = id
        };
        m_RendererData.LineVertices[m_RendererData.LineVertexCount + 1] = {
                .WorldPosition = end,
                .LocalPosition = end,
                .Color = color,
                .Id = id
        };

        m_RendererData.LineVertexCount += 2;
    }

    void ShapeRenderer::ReadPixel(int x, int y, const std::function<void(int32_t)> &callback)
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

    void ShapeRenderer::OnResize(uint32_t width, uint32_t height)
    {
        delete m_IdTexture;
        m_IdTexture = new WebGPUTexture(m_Context.GetDevice(), {
                .Format = WGPUTextureFormat_R32Sint,
                .Width = width,
                .Height = height,
                .Usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_CopySrc,
        });
    }
}
