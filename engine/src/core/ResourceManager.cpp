#include "ResourceManager.h"

namespace Figment
{
    Handle<WebGPUTexture> ResourceManager::CreateTexture(TextureDescriptor descriptor)
    {
        auto handle = m_TextureHandles.Get();
        // handle.data = new WebGPUTexture(m_Context.GetDevice(), descriptor.format, descriptor.width, descriptor.height);
        return handle;
    }
}
