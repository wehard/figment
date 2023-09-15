#pragma once

#include <webgpu/webgpu.h>
#include <vector>

template <typename T>
class WebGPUBuffer
{
public:
    WebGPUBuffer(WGPUDevice device, std::vector<T> data)
    {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.nextInChain = nullptr;
        bufferDesc.label = "WGPUBuffer";
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc;
        bufferDesc.size = data.size();
        bufferDesc.mappedAtCreation = false; // If true map RAM <-> VRAM
        m_Buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(queue, m_Buffer, 0, data.data(), data.size());
    }
    ~WebGPUBuffer<T>()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }
private:
    WGPUBuffer m_Buffer;
};

