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

    ~WebGPUBuffer<T>()
    {
        wgpuBufferRelease(m_Buffer);
        wgpuBufferDestroy(m_Buffer);
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

    using MapReadAsyncCallbackFn = std::function<void(const T*, size_t size)>;

    void MapReadAsync(MapReadAsyncCallbackFn callback)
    {
        auto mapState = GetMapState();
        if (mapState == WGPUBufferMapState_Mapped)
        {
            printf("Buffer is already mapped\n");
            return;
        }

        m_MapReadParams = { callback, this };

        wgpuBufferMapAsync(m_Buffer, WGPUMapMode_Read, 0, m_Size,
                [](WGPUBufferMapAsyncStatus status, void *userData)
                {
                    auto params = (MapReadParams *)userData;
                    if (status != WGPUBufferMapAsyncStatus_Success)
                    {
                        printf("Buffer map failed with WGPUBufferMapAsyncStatus: %d\n", status);
                        return;
                    }
                    auto *pixels = (T *)wgpuBufferGetConstMappedRange(params->Buffer->GetBuffer(), 0,
                            params->Buffer->GetSize());
                    wgpuBufferUnmap(params->Buffer->GetBuffer());
                    params->Callback(pixels, params->Buffer->GetSize());
                }, (void*)&m_MapReadParams);
    }


private:
    struct MapReadParams
    {
        MapReadAsyncCallbackFn Callback;
        WebGPUBuffer<T> *Buffer;
    };
    MapReadParams m_MapReadParams;
    WGPUDevice m_Device;
    WGPUBuffer m_Buffer;
    uint32_t m_Size;
};
