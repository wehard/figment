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

        m_MeshDataUniformBuffer = new WebGPUUniformBuffer<MeshData>(m_Context.GetDevice(),
                "MeshRendererMeshDataUniformBuffer",
                sizeof(MeshData));

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

        WGPURenderPassColorAttachment colorAttachments[1] = {};

        colorAttachments[0].loadOp = WGPULoadOp_Load;
        colorAttachments[0].storeOp = WGPUStoreOp_Store;
        colorAttachments[0].clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
        colorAttachments[0].view = wgpuSwapChainGetCurrentTextureView(m_Context.GetSwapChain());

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
        renderPassDesc.colorAttachments = colorAttachments;
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
        if (m_RenderPipeline == nullptr)
        {
            auto vertexLayout = mesh.VertexBuffer()->GetVertexLayout(); // TODO: Vertex layout should be defined here and only meshes with that layout should be rendered
            CreateDefaultPipeline(*m_DefaultShader, vertexLayout);
        }
        MeshData meshData = {
                .ModelMatrix = transform
        };
        m_MeshDataUniformBuffer->SetData(&meshData, sizeof(meshData));

        WebGPUCommand::DrawIndexed(m_RenderPassEncoder, m_RenderPipeline->Get(), m_BindGroup->Get(),
                *mesh.IndexBuffer(), *mesh.VertexBuffer(), mesh.IndexCount());
    }

    void MeshRenderer::CreateDefaultPipeline(WebGPUShader &shader, WGPUVertexBufferLayout &vertexBufferLayout)
    {
        m_BindGroup = new BindGroup(m_Context.GetDevice(), WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
        m_BindGroup->Bind(*m_CameraDataUniformBuffer);
        m_BindGroup->Bind(*m_MeshDataUniformBuffer);

        m_RenderPipeline = new RenderPipeline(m_Context.GetDevice(), shader, *m_BindGroup, vertexBufferLayout);
        m_RenderPipeline->AddColorTarget(m_Context.GetTextureFormat(), WGPUColorWriteMask_All);
        m_RenderPipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat());
        m_RenderPipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW, WGPUCullMode_None);
    }
}
