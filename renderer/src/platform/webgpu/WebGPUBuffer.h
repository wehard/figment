#pragma once

#include <webgpu/webgpu.h>
#include <vector>
#include <iostream>

template <typename T>
class WebGPUBuffer
{
public:
    WebGPUBuffer(WGPUDevice device, std::vector<T> data) : m_Size(data.size() * sizeof(T))
    {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.nextInChain = nullptr;
        bufferDesc.label = "WGPUBuffer";
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc;
        bufferDesc.size = m_Size;
        bufferDesc.mappedAtCreation = false; // If true map RAM <-> VRAM
        m_Buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(queue, m_Buffer, 0, data.data(), m_Size);

        printf("WebGPUBuffer<%s> created\n", typeid(T).name());
    }

    ~WebGPUBuffer<T>()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }
private:
    WGPUBuffer m_Buffer;
    uint32_t m_Size;
};

