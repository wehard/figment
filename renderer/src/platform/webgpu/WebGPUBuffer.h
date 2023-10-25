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

    WebGPUBuffer(WGPUDevice device, const char *label, uint64_t size, WGPUBufferUsageFlags usage) : m_Size(size)
    {
        WGPUBufferDescriptor pixelBufferDesc = {};
        pixelBufferDesc.nextInChain = nullptr;
        pixelBufferDesc.label = label;
        pixelBufferDesc.mappedAtCreation = false;
        pixelBufferDesc.usage = usage;
        pixelBufferDesc.size = size;
        m_Buffer = wgpuDeviceCreateBuffer(device, &pixelBufferDesc);
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

    ~WebGPUBuffer<T>()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }
private:
    WGPUBuffer m_Buffer;
    uint32_t m_Size;
};

class WebGPUVertexBuffer
{
public:
    WebGPUVertexBuffer(WGPUDevice device, std::vector<float> vertices) : m_Size(vertices.size() * sizeof(float)), m_VertexCount(vertices.size() / 3)
    {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.nextInChain = nullptr;
        bufferDesc.label = "VertexBuffer";
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
        bufferDesc.size = m_Size;
        bufferDesc.mappedAtCreation = false;
        m_Buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(queue, m_Buffer, 0, vertices.data(), m_Size);
    }

    ~WebGPUVertexBuffer()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }

    WGPUBuffer m_Buffer;
    uint32_t m_Size;
    uint32_t m_VertexCount;
};

class WebGPUIndexBuffer
{
public:
    WebGPUIndexBuffer(WGPUDevice device, std::vector<uint32_t> indices) : m_Size(indices.size() * sizeof(uint32_t)), m_IndexCount(indices.size())
    {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.nextInChain = nullptr;
        bufferDesc.label = "IndexBuffer";
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
        bufferDesc.size = m_Size;
        bufferDesc.mappedAtCreation = false;
        m_Buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(queue, m_Buffer, 0, indices.data(), m_Size);
    }

    ~WebGPUIndexBuffer()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }

    WGPUBuffer m_Buffer;
    uint32_t m_Size;
    uint32_t m_IndexCount;
};

template <typename T>
class WebGPUUniformBuffer
{
public:
    WebGPUUniformBuffer<T>(WGPUDevice device, T *data, uint32_t size) : m_Size(size)
    {
        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.nextInChain = nullptr;
        bufferDesc.label = "UniformBuffer";
        bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
        bufferDesc.size = m_Size;
        bufferDesc.mappedAtCreation = false;
        m_Buffer = wgpuDeviceCreateBuffer(device, &bufferDesc);

        WGPUQueue queue = wgpuDeviceGetQueue(device);
        wgpuQueueWriteBuffer(queue, m_Buffer, 0, (void*)data, m_Size);
    }

    ~WebGPUUniformBuffer<T>()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
    }

    WGPUBuffer m_Buffer;
    uint32_t m_Size;
};
