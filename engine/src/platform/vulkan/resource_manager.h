#pragma once

#include "Arena.h"
#include "Handle.h"
#include "Pool.h"
#include "buffer.h"
#include "context.h"

namespace figment::vulkan
{

class ResourceManager
{
public:
    struct PoolSizes
    {
        uint32_t Buffers      = 128;
        uint32_t RenderPasses = 128;
    };

    explicit ResourceManager(const Context& context, const PoolSizes& poolSizes):
        m_Context(context),
        m_Buffers(poolSizes.Buffers, [](Buffer* item) { delete item; })
        {};
    ~ResourceManager() = default;

    Handle<Buffer> CreateBuffer(const BufferDescriptor&& descriptor)
    {
        // return m_Buffers.Create(m_Context, std::move(descriptor));
        return Handle<Buffer>();
    }


private:
    const Context& m_Context;

    Pool<Buffer> m_Buffers;
};
} // namespace figment::vulkan
