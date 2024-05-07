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
        m_CameraDataUniformBuffer = std::make_unique<WebGPUUniformBuffer<CameraData>>(context.GetDevice(), "OverlayRendererCameraData",
                sizeof(CameraData));
        m_GridDataUniformBuffer = std::make_unique<WebGPUUniformBuffer<GridData>>(context.GetDevice(), "OverlayRendererGridData",
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

        m_GridPipeline = std::make_unique<RenderPipeline>(m_Context.GetDevice(), *m_GridShader, *m_GridBindGroup,
                m_GridMesh->VertexBuffer()->GetVertexLayout());
        m_GridPipeline->SetPrimitiveState(
                WGPUPrimitiveTopology_TriangleList,
                WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW,
                WGPUCullMode_None
        );
        m_GridPipeline->AddColorTarget(m_RenderTarget->Color.TextureFormat, WGPUColorWriteMask_All);
        m_GridPipeline->SetDepthStencilState(m_RenderTarget->Depth.TextureFormat);
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

        // WGPURenderPassColorAttachment colorAttachment = {};
        // colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        // colorAttachment.loadOp = WGPULoadOp_Clear;
        // colorAttachment.storeOp = WGPUStoreOp_Store;
        // colorAttachment.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
        // colorAttachment.view = m_RenderTarget->Color.TextureView;
        //
        // WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        // depthStencilAttachment.view = m_RenderTarget->Depth.TextureView;
        // depthStencilAttachment.depthClearValue = 1.0f;
        // depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        // depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        // depthStencilAttachment.depthReadOnly = false;
        // depthStencilAttachment.stencilClearValue = 0;
        // depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
        // depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
        // depthStencilAttachment.stencilReadOnly = true;

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
