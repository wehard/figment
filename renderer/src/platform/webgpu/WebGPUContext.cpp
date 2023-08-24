#include "WebGPUContext.h"

#include <emscripten/html5_webgpu.h>
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>

#include <cstdio>

void WebGPUContext::Init()
{
    m_WebGPUDevice = emscripten_webgpu_get_device();

    wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc = {};
    html_surface_desc.selector = "#canvas";

    wgpu::SurfaceDescriptor surface_desc = {};
    surface_desc.nextInChain = &html_surface_desc;

    WGPUInstanceDescriptor instance_desc = {};
    wgpu::Instance instance = wgpuCreateInstance(&instance_desc);

    m_WebGPUSurface = instance.CreateSurface(&surface_desc).Release();
    printf("Initialized WebGPU context\n");
}

void WebGPUContext::SwapBuffers()
{

}
