#include "WebGPUContext.h"

#include <emscripten/html5_webgpu.h>
#include <webgpu/webgpu.h>
// #include <webgpu/webgpu_cpp.h>

#include <stdio.h>
#include <vector>
#include <iostream>

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

void WebGPUContext::Init()
{

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
    surfaceDesc.nextInChain = (WGPUChainedStruct *) &htmlSurfaceDesc;

    WGPUInstanceDescriptor instance_desc = {};
    m_Instance = wgpuCreateInstance(&instance_desc);
    if (!m_Instance)
    {
        printf("Failed to create WebGPU instance!\n");
    }

    m_WebGPUSurface = wgpuInstanceCreateSurface(m_Instance, &surfaceDesc);
    if (!m_WebGPUSurface)
    {
        printf("Failed to create WebGPU surface!\n");
    }
    printf("Initialized WebGPU context\n");

    m_TextureFormat = WGPUTextureFormat_BGRA8Unorm;

    CreateSwapChain(width, height);
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
    {
        printf("Failed to create WebGPU swap chain!\n");
    }
}

void WebGPUContext::SwapBuffers()
{

}
