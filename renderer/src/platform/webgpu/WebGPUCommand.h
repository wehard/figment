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
        WGPUCommandEncoderDescriptor commandEncoderDesc = {};
        commandEncoderDesc.nextInChain = nullptr;
        commandEncoderDesc.label = "CopyCommandEncoder";
        WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(device, &commandEncoderDesc);

        wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, from.GetBuffer(), 0, to.GetBuffer(), 0, size);

        WGPUCommandBufferDescriptor commandBufferDesc = {};
        commandBufferDesc.nextInChain = nullptr;
        commandBufferDesc.label = "CopyCommandBuffer";

        auto commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDesc);
        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueSubmit(queue, 1, &commandBuffer);

        wgpuCommandEncoderRelease(commandEncoder);
    }
};


