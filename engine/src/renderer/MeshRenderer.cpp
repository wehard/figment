#include "MeshRenderer.h"
#include "WebGPUCommand.h"
#include "FileUtils.h"
#include "RenderStats.h"

namespace Figment
{

    MeshRenderer::MeshRenderer(WebGPUContext &context) : m_Context(context)
    {
        m_DefaultShader = new WebGPUShader(context.GetDevice(), "res/shaders/builtin/mesh.wgsl",
                "MeshRendererDefaultShader");

        m_TexturedShader = new WebGPUShader(context.GetDevice(), "res/shaders/builtin/mesh_textured.wgsl",
                "MeshRendererTexturedShader");

        m_CameraDataUniformBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(),
                "MeshRendererCameraDataUniformBuffer",
                sizeof(CameraData));

        m_DepthTexture = new WebGPUTexture(context.GetDevice(), WebGPUTextureDescriptor {
                .Format = WGPUTextureFormat_Depth24Plus,
                .Width = context.GetSwapChainWidth(),
                .Height = context.GetSwapChainHeight(),
                .Usage = WGPUTextureUsage_RenderAttachment,
                .Aspect = WGPUTextureAspect_DepthOnly,
                .Label = "MeshRendererDepthTexture"
        });
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
        colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        colorAttachment.loadOp = WGPULoadOp_Clear;
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

        for (auto &mrd : m_MeshRenderData)
        {
            mrd.second.InstanceCount = 0;
        }
    }

    void MeshRenderer::SubmitDrawCall(Mesh &mesh, MeshRenderData &data)
    {
        data.InstanceBuffer->SetData(data.InstanceDataStagingBuffer, sizeof(MeshInstanceData) * data.InstanceCount,
                0);
        wgpuRenderPassEncoderSetIndexBuffer(m_RenderPassEncoder, mesh.IndexBuffer()->GetBuffer(),
                WGPUIndexFormat_Uint32, 0,
                mesh.IndexBuffer()->GetSize());
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPassEncoder, 0, mesh.VertexBuffer()->GetBuffer(), 0,
                mesh.VertexBuffer()->GetSize());
        wgpuRenderPassEncoderSetVertexBuffer(m_RenderPassEncoder, 1, data.InstanceBuffer->GetBuffer(), 0,
                sizeof(MeshInstanceData) * data.InstanceCount);
        wgpuRenderPassEncoderSetPipeline(m_RenderPassEncoder, data.Pipeline->Get());
        wgpuRenderPassEncoderSetBindGroup(m_RenderPassEncoder, 0, data.BindGroup->Get(), 0, nullptr);
        wgpuRenderPassEncoderDrawIndexed(m_RenderPassEncoder, mesh.IndexCount(), data.InstanceCount, 0, 0, 0);
        RenderStats::VertexCount += mesh.VertexCount();
        RenderStats::DrawCalls++;
    }

    void MeshRenderer::EndFrame()
    {
        for (auto &mrd : m_MeshRenderData)
        {
            SubmitDrawCall(*mrd.first, mrd.second);
        }

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

    MeshRenderer::MeshRenderData MeshRenderer::CreateMeshRenderData(Mesh &mesh, WebGPUShader &shader,
            WebGPUTexture *texture)
    {
        MeshRenderData meshRenderData = {};
        meshRenderData.InstanceBuffer = new WebGPUVertexBuffer<MeshInstanceData>(m_Context.GetDevice(),
                "MeshRendererMeshDataInstanceBuffer",
                sizeof(MeshInstanceData) * MaxInstances);
        meshRenderData.InstanceDataStagingBuffer = new MeshInstanceData[MaxInstances];

        auto vertexAttributes = std::vector<WGPUVertexAttribute>({
                {
                        .format = WGPUVertexFormat_Float32x4,
                        .offset = 0,
                        .shaderLocation = 2,
                },
                {
                        .format = WGPUVertexFormat_Float32x4,
                        .offset = 16,
                        .shaderLocation = 3,
                },
                {
                        .format = WGPUVertexFormat_Float32x4,
                        .offset = 32,
                        .shaderLocation = 4,
                },
                {
                        .format = WGPUVertexFormat_Float32x4,
                        .offset = 48,
                        .shaderLocation = 5,
                }
        });
        meshRenderData.InstanceBuffer->SetVertexLayout(vertexAttributes, sizeof(MeshInstanceData),
                WGPUVertexStepMode_Instance);

        auto bindGroup = new BindGroup(m_Context.GetDevice(), WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
        bindGroup->Bind(*m_CameraDataUniformBuffer);
        if (texture != nullptr)
        {
            bindGroup->Bind(*texture);
        }

        auto pipeline = new RenderPipeline(m_Context.GetDevice(), shader, *bindGroup,
                { mesh.VertexBuffer()->GetVertexLayout(), meshRenderData.InstanceBuffer->GetVertexLayout() });
        pipeline->AddColorTarget(m_Context.GetTextureFormat(), WGPUColorWriteMask_All);
        pipeline->SetDepthStencilState(m_DepthTexture->GetTextureFormat());
        pipeline->SetPrimitiveState(WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined,
                WGPUFrontFace_CCW, WGPUCullMode_None);

        meshRenderData.Pipeline = pipeline;
        meshRenderData.BindGroup = bindGroup;
        return meshRenderData;
    }

    void MeshRenderer::Draw(Mesh &mesh, glm::mat4 transform)
    {
        Submit(mesh, transform, nullptr);
    }

    void MeshRenderer::DrawTextured(Mesh &mesh, glm::mat4 transform, WebGPUTexture &texture)
    {
        Submit(mesh, transform, &texture);
    }

    void MeshRenderer::Submit(Mesh &mesh, glm::mat4 transform, WebGPUTexture *texture)
    {
        if (m_MeshRenderData.find(&mesh) == m_MeshRenderData.end())
        {
            m_MeshRenderData[&mesh] = CreateMeshRenderData(mesh, *m_TexturedShader, texture);
        }

        auto &meshRenderData = m_MeshRenderData[&mesh];
        if (meshRenderData.InstanceCount >= MaxInstances)
        {
            return;
        }
        MeshInstanceData meshData = {
                .ModelMatrix = transform
        };
        meshRenderData.InstanceDataStagingBuffer[meshRenderData.InstanceCount] = meshData;
        meshRenderData.InstanceCount++;
    }

    void MeshRenderer::OnResize(uint32_t width, uint32_t height)
    {
        delete m_DepthTexture;

        m_DepthTexture = new WebGPUTexture(m_Context.GetDevice(), WebGPUTextureDescriptor {
                .Format = WGPUTextureFormat_Depth24Plus,
                .Width = width,
                .Height = height,
                .Usage = WGPUTextureUsage_RenderAttachment,
                .Aspect = WGPUTextureAspect_DepthOnly,
                .Label = "MeshRendererDepthTexture"
        });
    }
}
