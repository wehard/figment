#include <WebGPUCommand.h>
#include "OverlayRenderer.h"
#include "Mesh.h"

namespace Figment
{
    OverlayRenderer::OverlayRenderer(Figment::WebGPUContext &context) : m_Context(context),
            m_RenderTarget(context.GetDefaultRenderTarget())
    {
        m_GridShader = std::make_unique<WebGPUShader>(context.GetDevice(), "res/shaders/builtin/grid.wgsl",
                "OverlayRendererGridShader");
        m_CameraDataUniformBuffer = std::make_unique<WebGPUUniformBuffer<CameraData>>(context.GetDevice(),
                "OverlayRendererCameraData",
                sizeof(CameraData));
        m_GridDataUniformBuffer = std::make_unique<WebGPUUniformBuffer<GridData>>(context.GetDevice(),
                "OverlayRendererGridData",
                sizeof(GridData));

        m_GridBindGroup = std::make_unique<BindGroup>(m_Context.GetDevice(),
                WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
        m_GridBindGroup->Bind(*m_CameraDataUniformBuffer);
        m_GridBindGroup->Bind(*m_GridDataUniformBuffer);

        m_GridMesh = new Mesh(m_Context.GetDevice(), {
                {{ -1.0, -1.0, 0.0 }, { 0.0, 0.0 }},
                {{ -1.0, 1.0, 0.0 }, { 0.0, 1.0 }},
                {{ 1.0, 1.0, 0.0 }, { 1.0, 1.0 }},
                {{ 1.0, -1.0, 0.0 }, { 1.0, 0.0 }},
        }, {
                0, 2, 1, 0, 3, 2
        });

        m_GridMesh->VertexBuffer()->SetVertexLayout({
                {
                        .format = WGPUVertexFormat_Float32x3,
                        .offset = 0,
                        .shaderLocation = 0
                },
                {
                        .format = WGPUVertexFormat_Float32x2,
                        .offset = 12,
                        .shaderLocation = 1
                }
        }, sizeof(Mesh::Vertex), WGPUVertexStepMode_Vertex);

        m_GridPipeline = std::make_unique<RenderPipeline>(m_Context.GetDevice(), RenderPipelineDescriptor {
                .Shader = *m_GridShader,
                .BindGroup = *m_GridBindGroup,
                .VertexBufferLayouts = { m_GridMesh->VertexBuffer()->GetVertexLayout() },
                .ColorTargetStates = {
                        { m_RenderTarget->Color.TextureFormat, WGPUColorWriteMask_All }
                },
                .DepthStencilStates = {
                        { m_RenderTarget->Depth.TextureFormat, true, WGPUCompareFunction_Less }
                },
                .PrimitiveState = {
                        .topology = WGPUPrimitiveTopology_TriangleList,
                        .stripIndexFormat = WGPUIndexFormat_Undefined,
                        .frontFace = WGPUFrontFace_CCW,
                        .cullMode = WGPUCullMode_None
                }
        });
    }

    void OverlayRenderer::Begin(Camera &camera)
    {
        m_FrameCameraData = {
                .ViewMatrix = camera.GetViewMatrix(),
                .ProjectionMatrix = camera.GetProjectionMatrix()
        };
        m_CameraDataUniformBuffer->SetData(&m_FrameCameraData, sizeof(CameraData));

        m_FrameGridData = {
                .InverseViewMatrix = glm::inverse(camera.GetViewMatrix()),
                .InverseProjectionMatrix = glm::inverse(camera.GetProjectionMatrix())
        };
        m_GridDataUniformBuffer->SetData(&m_FrameGridData, sizeof(GridData));

        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(m_Context.GetDevice(), "OverlayRendererCommandEncoder");

        m_RenderTarget = m_Context.GetDefaultRenderTarget();

        auto colorAttachment = m_RenderTarget->GetColorAttachment();
        auto depthStencilAttachment = m_RenderTarget->GetDepthStencilAttachment();

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;
        renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

        m_RenderPassEncoder = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);
    }

    void OverlayRenderer::End()
    {
        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);
        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "OverlayRendererCommandBuffer");

        WebGPUCommand::SubmitCommandBuffer(m_Context.GetDevice(), commandBuffer);
        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);

        m_CommandEncoder = nullptr;
        m_RenderPassEncoder = nullptr;
    }

    void OverlayRenderer::DrawGrid()
    {
        WebGPUCommand::DrawIndexed(m_RenderPassEncoder, m_GridPipeline->Get(), m_GridBindGroup->Get(),
                *m_GridMesh->IndexBuffer(), *m_GridMesh->VertexBuffer(), m_GridMesh->IndexCount());
    }
}
