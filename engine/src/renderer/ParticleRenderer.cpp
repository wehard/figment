#include "ParticleRenderer.h"
#include "WebGPUCommand.h"
#include "RenderPipeline.h"

namespace Figment
{
    ParticleRenderer::ParticleRenderer(WebGPUContext &context) : m_Context(context), m_Device(context.GetDevice())
    {
        m_CameraDataUniformBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(),
                "ParticleRendererCameraDataUniformBuffer",
                sizeof(CameraData));

        m_ParticlesDataUniformBuffer = new WebGPUUniformBuffer<ParticlesData>(m_Context.GetDevice(),
                "ParticleRendererParticlesDataUniformBuffer",
                sizeof(ParticlesData));
    }

    ParticleRenderer::~ParticleRenderer()
    {
        delete m_CameraDataUniformBuffer;
    }

    void ParticleRenderer::BeginFrame(Camera &camera)
    {
        m_CommandEncoder = WebGPUCommand::CreateCommandEncoder(m_Device, "ParticleRendererCommandEncoder");

        CameraData cameraData = {
                .ViewMatrix = camera.GetViewMatrix(),
                .ProjectionMatrix = camera.GetProjectionMatrix()
        };

        m_CameraDataUniformBuffer->SetData(&cameraData, sizeof(CameraData));

        auto colorAttachment = m_Context.GetDefaultRenderTarget()->GetColorAttachment();
        auto depthStencilAttachment = m_Context.GetDefaultRenderTarget()->GetDepthStencilAttachment();

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;
        renderPassDesc.depthStencilAttachment = &depthStencilAttachment;

        m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);
    }

    void ParticleRenderer::EndFrame()
    {
        wgpuRenderPassEncoderEnd(m_RenderPass);

        auto commandBuffer = WebGPUCommand::CommandEncoderFinish(m_CommandEncoder,
                "ParticleRendererCommandBuffer");

        WebGPUCommand::SubmitCommandBuffer(m_Context.GetDevice(), commandBuffer);
        WebGPUCommand::DestroyCommandEncoder(m_CommandEncoder);
        WebGPUCommand::DestroyCommandBuffer(commandBuffer);
        wgpuRenderPassEncoderRelease(m_RenderPass);
        m_CommandEncoder = nullptr;
        m_RenderPass = nullptr;
    }

    void ParticleRenderer::OnResize(uint32_t width, uint32_t height)
    {
    }
}
