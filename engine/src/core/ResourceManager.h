#pragma once

#include <WebGPUContext.h>
#include "WebGPUTexture.h"
#include "RenderContext.h"
#include "Handle.h"

namespace Figment
{
    template<typename A>
    struct Array
    {
        uint32_t size = 0;
        uint32_t capacity = 0;
        A *data = nullptr;

        Array() = default;
        explicit Array(uint32_t capacity) : capacity(capacity)
        {
            data = new A[capacity];
        }

        A Get() {
            if (size >= capacity)
            {
                capacity *= 2;
                A *newData = new A[capacity];
                for (uint32_t i = 0; i < size; i++)
                {
                    newData[i] = data[i];
                }
                delete[] data;
                data = newData;
            }
            A handle = data[size];
            handle.index = size;
            size++;
            return handle;
        }

        void Delete(A handle) {
            for (uint32_t i = 0; i < size; i++)
            {
                if (&data[i].index == index)
                {
                    data[i].generation++;
                    return;
                }
            }
        }
    };

    struct TextureDescriptor
    {
        WGPUTextureDimension dimension = WGPUTextureDimension_2D;
        uint32_t size = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t depth = 1;
        uint32_t arrayLayerCount = 1;
        uint32_t mipLevelCount = 1;
        WGPUTextureFormat format = WGPUTextureFormat_RGBA8Unorm;
        WGPUTextureUsage usage = static_cast<WGPUTextureUsage>(WGPUTextureUsage_TextureBinding
                | WGPUTextureUsage_CopySrc);
    };

    class ResourceManager
    {
    public:
        explicit ResourceManager(WebGPUContext &context) : m_Context(context) {};
        ~ResourceManager() = default;

        Handle<WebGPUTexture> CreateTexture(TextureDescriptor descriptor);
    private:
        WebGPUContext &m_Context;

        Array<Handle<WebGPUTexture>> m_TextureHandles;
    };
}
