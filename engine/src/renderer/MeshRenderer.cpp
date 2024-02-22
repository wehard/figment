#include "MeshRenderer.h"
#include "WebGPUCommand.h"
#include "Utils.h"

namespace Figment
{

    MeshRenderer::MeshRenderer(WebGPUContext &context) : m_Context(context)
    {
        m_DefaultShader = new WebGPUShader(context.GetDevice(),
                *Utils::LoadFile2("res/shaders/mesh.wgsl"));

        m_CameraDataUniformBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(),
                "MeshRendererCameraDataUniformBuffer",
                sizeof(CameraData));

        m_DepthTexture = WebGPUTexture::CreateDepthTexture(context.GetDevice(), WGPUTextureFormat_Depth24Plus,
                context.GetSwapChainWidth(), context.GetSwapChainHeight());
    }

    void MeshRenderer::BeginFrame(Camera &camera)
    {
        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(m_Context.GetDevice(), "MeshRendererCommandEncoder");

        CameraData cameraData = {
                .ViewMatrix = camera.GetViewMatrix(),
                .ProjectionMatrix = camera.GetProjectionMatrix()
        };
        m_CameraDataUniformBuffer->SetData(&cameraData, sizeof(CameraData));

        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.loadOp = WGPULoadOp_Load;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
        colorAttachment.view = wgpuSwapChainGetCurrentTextureView(m_Context.GetSwapChain());

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
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;
        renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

        m_RenderPassEncoder = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);
    }

    void MeshRenderer::EndFrame()
    {
        wgpuRenderPassEncoderEnd(m_RenderPassEncoder);

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "RenderCommandBuffer");

        WebGPUCommand::SubmitCommandBuffer(m_Context.GetDevice(), commandBuffer);
        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuRenderPassEncoderRelease(m_RenderPassEncoder);

        m_CommandEncoder = nullptr;
        m_RenderPassEncoder = nullptr;
    }

    void MeshRenderer::Draw(Mesh &mesh, glm::mat4 transform)
    {
        if (m_MeshRenderData.find(&mesh) == m_MeshRenderData.end())
        {
            MeshRenderData mrd = {};
            mrd.UniformBuffer = new WebGPUUniformBuffer<MeshData>(m_Context.GetDevice(),
                    "MeshRendererMeshDataUniformBuffer",
                    sizeof(MeshData));

            auto vertexBufferLayout = mesh.VertexBuffer()->GetVertexLayout();
            auto bindGroup = new BindGroup(m_Context.GetDevice(), WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
            bindGroup->Bind(*m_CameraDataUniformBuffer);
            bindGroup->Bind(*mrd.UniformBuffer);

            auto pipeline = new RenderPipeline(m_Context.GetDevice(), *m_DefaultShader, *bindGroup, vertexBufferLayout);
            pipeline->AddColorTarget(m_Context.GetTextureFormat(), WGPUColorWriteMask_All);
            pipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat());
            pipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                    WGPUFrontFace_CCW, WGPUCullMode_None);

            mrd.Pipeline = pipeline;
            mrd.BindGroup = bindGroup;
            m_MeshRenderData[&mesh] = mrd;
        }

        auto &mrd = m_MeshRenderData[&mesh];
        MeshData meshData = {
                .ModelMatrix = transform
        };
        mrd.UniformBuffer->SetData(&meshData, sizeof(meshData));

        WebGPUCommand::DrawIndexed(m_RenderPassEncoder, mrd.Pipeline->Get(), mrd.BindGroup->Get(),
                *mesh.IndexBuffer(), *mesh.VertexBuffer(), mesh.IndexCount());
    }
}
