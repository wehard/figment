#pragma once

#include <webgpu/webgpu.h>

struct ColorTarget
{
    WGPUTextureView TextureView;
    WGPUTextureFormat TextureFormat;
};

struct DepthStencilTarget
{
    WGPUTextureView TextureView;
    WGPUTextureFormat TextureFormat;
};

struct RenderTarget
{
    ColorTarget Color;
    DepthStencilTarget Depth;
};
