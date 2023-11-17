#pragma once

#include "WebGPUBuffer.h"

class WebGPUCommand
{
public:
    WebGPUCommand() = delete;
    ~WebGPUCommand() = delete;
    
    template<typename T>
    static void CopyBufferToBuffer(WGPUDevice device, WebGPUBuffer<T> &from, WebGPUBuffer<T> &to, uint64_t size)
    {
        auto commandEncoder = CreateCommandEncoder(device, "CopyCommandEncoder");
        wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, from.GetBuffer(), 0, to.GetBuffer(), 0, size);
        auto commandBuffer = CreateCommandBuffer(device, commandEncoder, "CopyCommandBuffer");
        SubmitCommandBuffer(device, commandBuffer);
        DestroyCommandEncoder(commandEncoder);
        DestroyCommandBuffer(commandBuffer);
    }

    template<typename T>
    static void ClearBuffer(WGPUDevice device, WebGPUBuffer<T> &buffer)
    {
        auto commandEncoder = CreateCommandEncoder(device, "ClearBufferCommandEncoder");
        wgpuCommandEncoderClearBuffer(commandEncoder, buffer.GetBuffer(), 0, buffer.GetSize());
        auto commandBuffer = CreateCommandBuffer(device, commandEncoder, "ClearBufferCommandBuffer");
        SubmitCommandBuffer(device, commandBuffer);
        DestroyCommandEncoder(commandEncoder);
        DestroyCommandBuffer(commandBuffer);
    }

    static void SubmitCommandBuffer(WGPUDevice device, WGPUCommandBuffer commandBuffer)
    {
        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueSubmit(queue, 1, &commandBuffer);
    }

    static WGPUCommandEncoder CreateCommandEncoder(WGPUDevice device, const char *label = "CommandEncoder")
    {
        WGPUCommandEncoderDescriptor commandEncoderDesc = {};
        commandEncoderDesc.nextInChain = nullptr;
        commandEncoderDesc.label = label;
        return wgpuDeviceCreateCommandEncoder(device, &commandEncoderDesc);
    }

    static WGPUCommandBuffer CreateCommandBuffer(WGPUDevice device, WGPUCommandEncoder commandEncoder, const char *label = "CommandBuffer")
    {
        WGPUCommandBufferDescriptor commandBufferDesc = {};
        commandBufferDesc.nextInChain = nullptr;
        commandBufferDesc.label = label;
        return wgpuCommandEncoderFinish(commandEncoder, &commandBufferDesc);
    }

    static void DestroyCommandEncoder(WGPUCommandEncoder commandEncoder)
    {
        wgpuCommandEncoderRelease(commandEncoder);
    }

    static void DestroyCommandBuffer(WGPUCommandBuffer commandBuffer)
    {
        wgpuCommandBufferRelease(commandBuffer);
    }
};


