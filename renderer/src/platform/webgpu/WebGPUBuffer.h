#pragma once

#include <webgpu/webgpu.h>
#include <vector>
#include <iostream>

template<typename T>
class WebGPUBuffer
{
public:
    WebGPUBuffer(WGPUDevice device, const char *label, uint64_t size, WGPUBufferUsageFlags usage)
            : m_Device(device), m_Size(size)
    {
        WGPUBufferDescriptor pixelBufferDesc = {};
        pixelBufferDesc.nextInChain = nullptr;
        pixelBufferDesc.label = label;
        pixelBufferDesc.mappedAtCreation = false;
        pixelBufferDesc.usage = usage;
        pixelBufferDesc.size = size;
        m_Buffer = wgpuDeviceCreateBuffer(device, &pixelBufferDesc);
    }

    void SetData(T *data, uint32_t size)
    {
        WGPUQueue queue = wgpuDeviceGetQueue(m_Device);
        wgpuQueueWriteBuffer(queue, m_Buffer, 0, (void *)data, size);
    }

    WGPUBufferMapState GetMapState() const
    {
        return wgpuBufferGetMapState(m_Buffer);
    }

    WGPUBuffer GetBuffer() const
    {
        return m_Buffer;
    }

    void Unmap()
    {
        wgpuBufferUnmap(m_Buffer);
    }

    uint32_t GetSize() const
    {
        return m_Size;
    }

    ~WebGPUBuffer<T>()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }
private:
    WGPUDevice m_Device;
    WGPUBuffer m_Buffer;
    uint32_t m_Size;
};
