#include "ResourceManager.h"

#ifdef __EMSCRIPTEN__
#include "WebGPUContext.h"
#include "WebGPUBuffer.h"
#endif

namespace Figment
{
    Handle<Texture> ResourceManager::CreateTexture(const TextureDescriptor &&descriptor)
    {
        auto handle = m_TextureHandles.Create({ .width = descriptor.width, .height = descriptor.height });
        return handle;
    }

    Handle<BindGroup> ResourceManager::CreateBindGroup(const BindGroupDescriptor &&descriptor)
    {
        auto handle = m_BindGroupHandles.Create({ .size = descriptor.size });
        // handle.data = new WebGPUBindGroup(m_Context.GetDevice(), descriptor.size);
        return handle;
    }

    Handle<Buffer> ResourceManager::CreateBuffer(const BufferDescriptor &&descriptor)
    {
#ifdef __EMSCRIPTEN__
        auto webGPUContext = dynamic_cast<WebGPUContext *>(&m_Context);
        auto buffer = new WebGPUBuffer<uint32_t>(webGPUContext->GetDevice(), "", descriptor.ByteSize, 0);
#endif
        // auto buffer = Buffer::Create(descriptor.ByteSize, descriptor.Usage);
        auto handle = m_BufferHandles.Create({});
        // handle.data = *buffer;
        return handle;
    }
}
