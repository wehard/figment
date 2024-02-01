#pragma once

#include "Log.h"
#include <webgpu/webgpu.h>
#include <vector>
#include <iostream>
#include <functional>

namespace Figment
{
    template<typename T>
    class WebGPUBuffer
    {
    public:
        WebGPUBuffer(WGPUDevice device, const char *label, uint64_t size, WGPUBufferUsageFlags usage)
                : m_Device(device), m_Size(size), m_Label(label)
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

        using MapReadAsyncCallbackFn = std::function<void(const T *, size_t size)>;

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
                            auto label = params->Buffer->m_Label;
                            FIG_LOG_WARN("WebGPUBuffer \"%s\" failed with WGPUBufferMapAsyncStatus: %d", label, status);
                            return;
                        }
                        auto *data = (T *)wgpuBufferGetConstMappedRange(params->Buffer->GetBuffer(), 0,
                                params->Buffer->GetSize());
                        params->Callback(data, params->Buffer->GetSize());
                        wgpuBufferUnmap(params->Buffer->GetBuffer());
                    }, (void *)&m_MapReadParams);
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
        const char *m_Label;
    };

    static WGPUBindGroupEntry GetDefaultWGPUBindGroupEntry()
    {
        WGPUBindGroupEntry bindingLayout = {};
        bindingLayout.nextInChain = nullptr;
        bindingLayout.binding = 0;
        bindingLayout.buffer = nullptr;
        bindingLayout.offset = 0;
        bindingLayout.size = 0;
        bindingLayout.sampler = nullptr;
        bindingLayout.textureView = nullptr;

        return bindingLayout;
    }

    static WGPUBindGroupLayoutEntry GetDefaultWGPUBindGroupLayoutEntry()
    {
        WGPUBindGroupLayoutEntry bindingLayout = {};

        bindingLayout.buffer.nextInChain = nullptr;
        bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
        bindingLayout.buffer.hasDynamicOffset = false;

        bindingLayout.sampler.nextInChain = nullptr;
        bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

        bindingLayout.storageTexture.nextInChain = nullptr;
        bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
        bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
        bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

        bindingLayout.texture.nextInChain = nullptr;
        bindingLayout.texture.multisampled = false;
        bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
        bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;

        return bindingLayout;
    }

    template<typename T>
    class WebGPUUniformBuffer : public WebGPUBuffer<T>
    {
    public:
        WebGPUUniformBuffer(WGPUDevice device, const char *label, uint64_t size) : WebGPUBuffer<T>(device, label, size,
                WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform)
        { };

        WGPUBindGroupEntry GetBindGroupEntry(uint32_t binding, uint32_t offset)
        {
            WGPUBindGroupEntry entry = {
                    .nextInChain = nullptr,
                    .binding = binding,
                    .buffer = this->GetBuffer(),
                    .offset = offset,
                    .size = this->GetSize(),
                    .sampler = nullptr,
                    .textureView = nullptr,
            };
            return entry;
        }

        WGPUBindGroupLayoutEntry GetBindGroupLayoutEntry(uint32_t binding,
                WGPUShaderStageFlags visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment)
        {
            WGPUBindGroupLayoutEntry entry = GetDefaultWGPUBindGroupLayoutEntry();
            entry.buffer.type = WGPUBufferBindingType_Uniform;
            entry.buffer.minBindingSize = this->GetSize();
            entry.visibility = visibility;
            entry.binding = binding;
            return entry;
        }
    };

    template<typename T>
    class WebGPUVertexBuffer : public WebGPUBuffer<T>
    {
    public:
        WebGPUVertexBuffer(WGPUDevice device, const char *label, uint64_t size) : WebGPUBuffer<T>(device, label, size,
                WGPUBufferUsage_Storage | WGPUBufferUsage_CopySrc | WGPUBufferUsage_CopyDst
                        | WGPUBufferUsage_Vertex), m_Count(size / sizeof(T))
        { };

        void SetVertexLayout(std::vector<WGPUVertexAttribute> vertexAttributes, uint64_t stride,
                WGPUVertexStepMode stepMode)
        {
            m_VertexAttributes = vertexAttributes;
            m_VertexBufferLayout.attributeCount = m_VertexAttributes.size();
            m_VertexBufferLayout.attributes = m_VertexAttributes.data();
            m_VertexBufferLayout.arrayStride = stride; // TODO: stride should be calculated from attributes
            m_VertexBufferLayout.stepMode = stepMode;
        }

        WGPUVertexBufferLayout GetVertexLayout()
        {
            return m_VertexBufferLayout;
        }

        uint32_t Count()
        {
            return m_Count;
        }

    private:
        std::vector<WGPUVertexAttribute> m_VertexAttributes;
        WGPUVertexBufferLayout m_VertexBufferLayout = {};
        uint32_t m_Count;
    };

    template<typename T>
    class WebGPUIndexBuffer : public WebGPUBuffer<T>
    {
    public:
        WebGPUIndexBuffer(WGPUDevice device, const char *label, uint64_t size) : WebGPUBuffer<uint32_t>(device, label,
                size,
                WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index), m_Count(size / sizeof(T))
        { };

        uint32_t Count()
        {
            return m_Count;
        }

    private:
        std::vector<WGPUVertexAttribute> m_VertexAttributes;
        uint32_t m_Count;
    };
}
