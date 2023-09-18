#include "WebGPURenderer.h"
#include "WebGPUBuffer.h"
#include <vector>

const char* shaderSource = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
    var p = vec2f(0.0, 0.0);
    if (in_vertex_index == 0u) {
        p = vec2f(-0.5, -0.5);
    } else if (in_vertex_index == 1u) {
        p = vec2f(0.5, -0.5);
    } else {
        p = vec2f(0.0, 0.5);
    }
    return vec4f(p, 0.0, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return vec4f(0.0, 0.4, 0.6, 1.0);
}
)";

static WGPUShaderModule CreateShaderModule(WGPUDevice device)
{
    WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {};
    shaderCodeDesc.chain.next = nullptr;
    shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
    shaderCodeDesc.code = shaderSource;

    WGPUShaderModuleDescriptor shaderDesc = {};
    shaderDesc.nextInChain = &shaderCodeDesc.chain;

    WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);
    return shaderModule;
}

WebGPURenderer::WebGPURenderer(WebGPUContext& context) : m_Context(context)
{
    m_ShaderModule = CreateShaderModule(context.GetDevice());

    std::vector<int> data = {1, 2, 3, 4};
    auto *buffer = new WebGPUBuffer<int>(context.GetDevice(), data);

}

WGPURenderPassEncoder WebGPURenderer::Begin()
{
    auto device = m_Context.GetDevice();

    WGPUCommandEncoderDescriptor desc = {};
    m_CommandEncoder = wgpuDeviceCreateCommandEncoder(device, &desc);

    WGPURenderPassColorAttachment colorAttachment = {};
    colorAttachment.loadOp = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;
    colorAttachment.clearValue = {0.1, 0.1, 0.1, 1};
    colorAttachment.view = wgpuSwapChainGetCurrentTextureView(m_Context.GetSwapChain());

    WGPURenderPassDescriptor renderPassDesc = {};
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttachment;
    renderPassDesc.depthStencilAttachment = nullptr;

    m_RenderPass = wgpuCommandEncoderBeginRenderPass(m_CommandEncoder, &renderPassDesc);
    return m_RenderPass;
}

void WebGPURenderer::End()
{
    wgpuRenderPassEncoderEnd(m_RenderPass);

    WGPUCommandBufferDescriptor commandBufferDesc = {};
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(m_CommandEncoder, &commandBufferDesc);
    WGPUQueue queue = wgpuDeviceGetQueue(m_Context.GetDevice());
    wgpuQueueSubmit(queue, 1, &commandBuffer);

    wgpuCommandEncoderRelease(m_CommandEncoder);
    m_CommandEncoder = nullptr;
    wgpuRenderPassEncoderRelease(m_RenderPass);
    m_RenderPass = nullptr;
}

void WebGPURenderer::DrawQuad(glm::mat4 transform, glm::vec4 color)
{
    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.nextInChain = nullptr;
    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers = nullptr;

    pipelineDesc.vertex.module = m_ShaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;

    pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
    pipelineDesc.primitive.cullMode = WGPUCullMode_None;

    WGPUBlendState blendState = {};
    blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blendState.color.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState colorTarget = {};
    colorTarget.format = m_Context.GetTextureFormat();
    colorTarget.blend = &blendState;
    colorTarget.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragmentState = {};
    fragmentState.module = m_ShaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    pipelineDesc.fragment = &fragmentState;
    pipelineDesc.depthStencil = nullptr;

    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    pipelineDesc.layout = nullptr;

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &pipelineDesc);

    wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipeline);
    wgpuRenderPassEncoderDraw(m_RenderPass, 3, 1, 0, 0);
}
