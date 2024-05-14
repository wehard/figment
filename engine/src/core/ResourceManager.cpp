#include "ResourceManager.h"

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
}
