#include "WebGPURenderer.h"
#include "WebGPUBuffer.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>

const char* shaderSource = R"(
struct MVP {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>
};

@binding(0) @group(0) var<uniform> mvp: MVP;

@vertex
fn vs_main(@location(0) in_vertex_position: vec3f) -> @builtin(position) vec4f {
    return mvp.proj * mvp.view * mvp.model * vec4f(in_vertex_position, 1.0);
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

    std::vector<float> data = {-0.5, -0.5, 0.0,
                               +0.5, -0.5, 0.0,
                               -0.5, +0.5, 0.0,
                               +0.5, -0.5, 0.0,
                               -0.5, +0.5, 0.0,
                               +0.5, +0.5, 0.0
    };
    m_VertexBuffer = new WebGPUVertexBuffer(context.GetDevice(), data);
}

WGPURenderPassEncoder WebGPURenderer::Begin(Camera &camera)
{
    m_RenderPassData.ProjectionMatrix = camera.GetProjectionMatrix();
    m_RenderPassData.ViewMatrix = camera.GetViewMatrix();

    MVP mvp = {};
    mvp.model = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
    mvp.view = m_RenderPassData.ViewMatrix;
    mvp.proj = m_RenderPassData.ProjectionMatrix;
    m_UniformBuffer = new WebGPUUniformBuffer<MVP>(m_Context.GetDevice(), &mvp, sizeof(mvp));

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

static WGPUBindGroupLayoutEntry GetDefaultWGPUBindGroupLayoutEntry()
{
    WGPUBindGroupLayoutEntry bindingLayout = {};

    bindingLayout.buffer.nextInChain = nullptr;
    bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
    bindingLayout.buffer.hasDynamicOffset = false;

    bindingLayout.sampler.nextInChain = nullptr;
    bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

    bindingLayout.storageTexture.nextInChain = nullptr;
    bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
    bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
    bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

    bindingLayout.texture.nextInChain = nullptr;
    bindingLayout.texture.multisampled = false;
    bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
    bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;

    return bindingLayout;
}

void WebGPURenderer::DrawQuad(glm::mat4 transform, glm::vec4 color)
{
    WGPURenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.nextInChain = nullptr;

    WGPUVertexBufferLayout vertexBufferLayout = {};

    WGPUVertexAttribute vertexAttrib = {};
    vertexAttrib.shaderLocation = 0;
    vertexAttrib.format = WGPUVertexFormat_Float32x3;
    vertexAttrib.offset = 0;

    vertexBufferLayout.attributeCount = 1;
    vertexBufferLayout.attributes = &vertexAttrib;
    vertexBufferLayout.arrayStride = 3 * sizeof(float);
    vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;

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

    WGPUBindGroupLayoutEntry bindingLayout = GetDefaultWGPUBindGroupLayoutEntry();
    bindingLayout.binding = 0;
    bindingLayout.visibility = WGPUShaderStage_Vertex;
    bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
    bindingLayout.buffer.minBindingSize = m_UniformBuffer->m_Size;

    WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
    bindGroupLayoutDesc.nextInChain = nullptr;
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindingLayout;
    WGPUBindGroupLayout bindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Context.GetDevice(), &bindGroupLayoutDesc);

    WGPUPipelineLayoutDescriptor layoutDesc = {};
    layoutDesc.nextInChain = nullptr;
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &bindGroupLayout;
    WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(m_Context.GetDevice(), &layoutDesc);

    pipelineDesc.layout = layout;

    WGPUBindGroupEntry binding = {};
    binding.nextInChain = nullptr;
    binding.binding = 0;
    binding.buffer = m_UniformBuffer->m_Buffer;
    binding.offset = 0;
    binding.size = m_UniformBuffer->m_Size;

    WGPUBindGroupDescriptor bindGroupDesc = {};
    bindGroupDesc.nextInChain = nullptr;
    bindGroupDesc.layout = bindGroupLayout;
    bindGroupDesc.entryCount = bindGroupLayoutDesc.entryCount;
    bindGroupDesc.entries = &binding;
    WGPUBindGroup bindGroup = wgpuDeviceCreateBindGroup(m_Context.GetDevice(), &bindGroupDesc);

    WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(m_Context.GetDevice(), &pipelineDesc);

    wgpuRenderPassEncoderSetPipeline(m_RenderPass, pipeline);

    wgpuRenderPassEncoderSetVertexBuffer(m_RenderPass, 0, m_VertexBuffer->m_Buffer, 0, m_VertexBuffer->m_Size);

    wgpuRenderPassEncoderSetBindGroup(m_RenderPass, 0, bindGroup, 0, nullptr);

    wgpuRenderPassEncoderDraw(m_RenderPass, m_VertexBuffer->m_VertexCount, 1, 0, 0);
}
