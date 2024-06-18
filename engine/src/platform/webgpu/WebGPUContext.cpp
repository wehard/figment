#include "WebGPUContext.h"
#include "Log.h"
#include "WebGPUTexture.h"

#include <emscripten/html5_webgpu.h>
#include <webgpu/webgpu.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <webgpu/webgpu_cpp.h>

namespace Figment
{
    static void PrintWebGPUError(WGPUErrorType error_type, const char *message, void *)
    {
        const char *error_type_lbl = "";
        switch (error_type)
        {
        case WGPUErrorType_Validation:
            error_type_lbl = "Validation";
            break;
        case WGPUErrorType_OutOfMemory:
            error_type_lbl = "Out of memory";
            break;
        case WGPUErrorType_Unknown:
            error_type_lbl = "Unknown";
            break;
        case WGPUErrorType_DeviceLost:
            error_type_lbl = "Device lost";
            break;
        default:
            error_type_lbl = "Unknown";
        }
        printf("%s error: %s\n", error_type_lbl, message);
    }

    void WebGPUContext::Init(uint32_t width, uint32_t height)
    {
        m_WebGPUDevice = emscripten_webgpu_get_device();

        if (!m_WebGPUDevice)
        {
            printf("No device!\n");
            return;
        }
        wgpuDeviceSetLabel(m_WebGPUDevice, "WebGPUDevice");
        std::vector<WGPUFeatureName> features;
        size_t count = wgpuDeviceEnumerateFeatures(m_WebGPUDevice, nullptr);
        features.resize(count);
        wgpuDeviceEnumerateFeatures(m_WebGPUDevice, features.data());
        std::cout << "Device features:" << std::endl;
        for (auto f : features)
        {
            std::cout << " - " << f << std::endl;
        }

        wgpuDeviceSetUncapturedErrorCallback(m_WebGPUDevice, PrintWebGPUError, nullptr);

        WGPUSurfaceDescriptorFromCanvasHTMLSelector htmlSurfaceDesc = {};
        htmlSurfaceDesc.selector = "#canvas";
        htmlSurfaceDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;

        WGPUSurfaceDescriptor surfaceDesc = {};
        surfaceDesc.nextInChain = (WGPUChainedStruct *)&htmlSurfaceDesc;

        WGPUInstanceDescriptor instance_desc = {};
        instance_desc.nextInChain = nullptr;
        wgpu::Instance instance = wgpu::CreateInstance();
        m_Instance = instance.Get();
        if (!m_Instance)
        {
            FIG_LOG_ERROR("Failed to create WebGPU instance!");
        }

        auto hasStorageTextureReadWriteFeature = wgpuInstanceHasWGSLLanguageFeature(m_Instance,
                WGPUWGSLFeatureName_ReadonlyAndReadwriteStorageTextures);
        if (!hasStorageTextureReadWriteFeature)
        {
            FIG_LOG_ERROR("Storage texture read-write feature not supported!");
        }

        m_WebGPUSurface = wgpuInstanceCreateSurface(m_Instance, &surfaceDesc);
        if (!m_WebGPUSurface)
        {
            FIG_LOG_ERROR("Failed to create WebGPU surface!");
        }

        m_TextureFormat = WGPUTextureFormat_BGRA8Unorm;
        CreateSwapChain(width, height);
        FIG_LOG_INFO("WebGPU context initialized");
    }

    void WebGPUContext::CreateSwapChain(uint32_t width, uint32_t height)
    {
        m_SwapChainWidth = width;
        m_SwapChainHeight = height;

        if (m_SwapChain != nullptr)
        {
            wgpuSwapChainRelease(m_SwapChain);
        }

        WGPUSwapChainDescriptor swapChainDesc = {};
        swapChainDesc.usage = WGPUTextureUsage_RenderAttachment;
        swapChainDesc.label = "WebGPUContext::SwapChain";
        swapChainDesc.format = m_TextureFormat;
        swapChainDesc.width = m_SwapChainWidth;
        swapChainDesc.height = m_SwapChainHeight;
        swapChainDesc.presentMode = WGPUPresentMode_Fifo;
        m_SwapChain = wgpuDeviceCreateSwapChain(m_WebGPUDevice, m_WebGPUSurface, &swapChainDesc);

        if (!m_SwapChain)
            FIG_LOG_ERROR("Failed to create WebGPU swap chain!\n");

        // FIG_LOG_INFO("Created WebGPU swap chain (%dx%d)", m_SwapChainWidth, m_SwapChainHeight);

        CreateDefaultRenderTarget();
    }

    void WebGPUContext::CreateDefaultRenderTarget()
    {
        if (m_DepthTexture != nullptr)
        {
            delete m_DepthTexture;
        }
        m_DepthTexture = new WebGPUTexture(m_WebGPUDevice, {
                .Format = WGPUTextureFormat_Depth24Plus,
                .Width = m_SwapChainWidth,
                .Height = m_SwapChainHeight,
                .Usage = WGPUTextureUsage_RenderAttachment,
                .Aspect = WGPUTextureAspect_DepthOnly,
                .Label = "DepthTexture"
        });
        m_DefaultRenderTarget = {
                .Color = {
                        .TextureView = wgpuSwapChainGetCurrentTextureView(m_SwapChain),
                        .TextureFormat = m_TextureFormat,
                },
                .Depth = {
                        .TextureView = m_DepthTexture->GetTextureView(),
                        .TextureFormat = m_DepthTexture->GetTextureFormat(),
                }
        };
        // FIG_LOG_INFO("Created default render target");
    }

    // TODO: Figure out where this belongs
    void WebGPUContext::BeginFrame()
    {
        m_DefaultRenderTarget.Color.TextureView = wgpuSwapChainGetCurrentTextureView(m_SwapChain);
        Clear({ 0.1f, 0.1f, 0.1f, 1.0f });
    }

    void WebGPUContext::EndFrame()
    {
        wgpuTextureViewRelease(m_DefaultRenderTarget.Color.TextureView);
    }

    void WebGPUContext::Clear(WGPUColor clearColor)
    {
        WGPUCommandEncoderDescriptor commandEncoderDesc = {};
        commandEncoderDesc.nextInChain = nullptr;
        commandEncoderDesc.label = "WebGPUContext::CommandEncoder";
        auto commandEncoder = wgpuDeviceCreateCommandEncoder(m_WebGPUDevice, &commandEncoderDesc);

        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        colorAttachment.loadOp = WGPULoadOp_Clear;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = clearColor;
        colorAttachment.view = m_DefaultRenderTarget.Color.TextureView;
        colorAttachment.resolveTarget = nullptr;

        WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        depthStencilAttachment.view = m_DefaultRenderTarget.Depth.TextureView;
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

        auto renderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);
        wgpuRenderPassEncoderEnd(renderPass);

        WGPUCommandBufferDescriptor commandBufferDesc = {};
        commandBufferDesc.nextInChain = nullptr;
        commandBufferDesc.label = "WebGPUContext::CommandBuffer";
        auto commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(m_WebGPUDevice);
        wgpuQueueSubmit(queue, 1, &commandBuffer);

        wgpuCommandEncoderRelease(commandEncoder);
        wgpuCommandBufferRelease(commandBuffer);
        wgpuRenderPassEncoderRelease(renderPass);
    }

    void WebGPUContext::OnResize(uint32_t width, uint32_t height) { }
}
