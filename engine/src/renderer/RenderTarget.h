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

    WGPURenderPassColorAttachment GetColorAttachment()
    {
        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        colorAttachment.loadOp = WGPULoadOp_Load;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
        colorAttachment.view = Color.TextureView;

        return colorAttachment;
    }

    WGPURenderPassDepthStencilAttachment GetDepthStencilAttachment()
    {
        WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        depthStencilAttachment.view = Depth.TextureView;
        depthStencilAttachment.depthClearValue = 1.0f;
        depthStencilAttachment.depthLoadOp = WGPULoadOp_Load;
        depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.depthReadOnly = false;
        depthStencilAttachment.stencilClearValue = 0;
        depthStencilAttachment.stencilLoadOp = WGPULoadOp_Undefined;
        depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Undefined;
        depthStencilAttachment.stencilReadOnly = true;

        return depthStencilAttachment;
    }
};
