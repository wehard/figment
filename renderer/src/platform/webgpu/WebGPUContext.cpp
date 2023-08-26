#include "WebGPUContext.h"

#include <emscripten/html5_webgpu.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>

#include <stdio.h>

static void PrintWebGPUError(WGPUErrorType error_type, const char* message, void*)
{
    const char* error_type_lbl = "";
    switch (error_type)
    {
        case WGPUErrorType_Validation:  error_type_lbl = "Validation"; break;
        case WGPUErrorType_OutOfMemory: error_type_lbl = "Out of memory"; break;
        case WGPUErrorType_Unknown:     error_type_lbl = "Unknown"; break;
        case WGPUErrorType_DeviceLost:  error_type_lbl = "Device lost"; break;
        default:                        error_type_lbl = "Unknown";
    }
    printf("%s error: %s\n", error_type_lbl, message);
}

void WebGPUContext::Init()
{
    m_WebGPUDevice = emscripten_webgpu_get_device();
    if (!m_WebGPUDevice)
    {
        printf("No device!\n");
        return;
    }
    wgpuDeviceSetLabel(m_WebGPUDevice, "WebGPUDevice");

    wgpuDeviceSetUncapturedErrorCallback(m_WebGPUDevice, PrintWebGPUError, nullptr);

    wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
    html_surface_desc.selector = "#canvas";

    wgpu::SurfaceDescriptor surface_desc = {};
    surface_desc.nextInChain = &html_surface_desc;

    WGPUInstanceDescriptor instance_desc = {};
    wgpu::Instance instance = wgpuCreateInstance(&instance_desc);
    if (!instance)
    {
        printf("No instance!\n");
    }

    m_WebGPUSurface = instance.CreateSurface(&surface_desc).Release();
    if (!m_WebGPUSurface)
    {
        printf("No surface!\n");
    }
    printf("Initialized WebGPU context\n");
}

void WebGPUContext::SwapBuffers()
{

}
