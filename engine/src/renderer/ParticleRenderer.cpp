#include "ParticleRenderer.h"
#include "WebGPUCommand.h"

namespace Figment
{

    ParticleRenderer::ParticleRenderer(WebGPUContext &context) : m_Context(context), m_Device(context.GetDevice())
    {
        m_RenderTarget = wgpuSwapChainGetCurrentTextureView(m_Context.GetSwapChain());
        m_RenderTargetTextureFormat = m_Context.GetTextureFormat();
        m_DepthTexture = WebGPUTexture::CreateDepthTexture(context.GetDevice(), WGPUTextureFormat_Depth24Plus,
                context.GetSwapChainWidth(), context.GetSwapChainHeight());

        m_CameraDataUniformBuffer = new WebGPUUniformBuffer<CameraData>(m_Context.GetDevice(),
                "ParticleRendererCameraDataUniformBuffer",
                sizeof(CameraData));
    }

    ParticleRenderer::~ParticleRenderer()
    {
        delete m_DepthTexture;
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
        delete m_DepthTexture;
        m_DepthTexture = WebGPUTexture::CreateDepthTexture(m_Context.GetDevice(), WGPUTextureFormat_Depth24Plus,
                width, height);
    }

}