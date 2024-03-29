#include "BindGroup.h"

namespace Figment
{

    BindGroup::BindGroup(WGPUDevice device, WGPUShaderStageFlags visibility) : m_Device(device),
            m_Visibility(visibility)
    {
    }

    BindGroup::~BindGroup()
    {
    }

    WGPUBindGroup BindGroup::Get()
    {
        if (m_BindGroup != nullptr)
            return m_BindGroup;

        auto layout = GetLayout();

        WGPUBindGroupDescriptor bindGroupDesc = {};
        bindGroupDesc.label = "BindGroup";
        bindGroupDesc.layout = layout;
        bindGroupDesc.entryCount = m_BindGroupEntries.size();
        bindGroupDesc.entries = m_BindGroupEntries.data();
        m_BindGroup = wgpuDeviceCreateBindGroup(m_Device, &bindGroupDesc);
        return m_BindGroup;
    }

    WGPUBindGroupLayout BindGroup::GetLayout()
    {
        if (m_BindGroupLayout != nullptr)
            return m_BindGroupLayout;

        WGPUBindGroupLayoutDescriptor bindGroupLayoutDesc = {};
        bindGroupLayoutDesc.label = "BindGroupLayout";
        bindGroupLayoutDesc.entryCount = m_BindGroupLayoutEntries.size();
        bindGroupLayoutDesc.entries = m_BindGroupLayoutEntries.data();

        m_BindGroupLayout = wgpuDeviceCreateBindGroupLayout(m_Device, &bindGroupLayoutDesc);
        return m_BindGroupLayout;
    }

    void BindGroup::Bind(WGPUBuffer buffer, uint32_t size, WGPUBufferBindingType type)
    {
        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.buffer = buffer;
        bindGroupEntry.offset = 0;
        bindGroupEntry.size = size;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = m_Visibility;
        bindGroupLayoutEntry.buffer.type = type;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }

    void BindGroup::BindTexture(WGPUTextureView textureView, uint64_t size)
    {
        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.size = size;
        bindGroupEntry.offset = 0;
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.textureView = textureView;
        bindGroupEntry.buffer = nullptr;
        bindGroupEntry.sampler = nullptr;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = m_Visibility;
        bindGroupLayoutEntry.texture.sampleType = WGPUTextureSampleType_Float;
        bindGroupLayoutEntry.texture.viewDimension = WGPUTextureViewDimension_2D;
        bindGroupLayoutEntry.texture.multisampled = false;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }

    void BindGroup::BindStorageTexture(WGPUTextureView textureView, uint64_t size, WGPUTextureFormat format,
            WGPUStorageTextureAccess access)
    {
        WGPUBindGroupEntry bindGroupEntry = {};
        bindGroupEntry.size = size;
        bindGroupEntry.offset = 0;
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.textureView = textureView;
        bindGroupEntry.buffer = nullptr;
        bindGroupEntry.sampler = nullptr;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = m_Visibility;
        bindGroupLayoutEntry.storageTexture.nextInChain = nullptr;
        bindGroupLayoutEntry.storageTexture.format = format;
        bindGroupLayoutEntry.storageTexture.access = access;
        bindGroupLayoutEntry.storageTexture.viewDimension = WGPUTextureViewDimension_2D;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }

    void BindGroup::BindSampler(WGPUSampler sampler)
    {
        WGPUBindGroupEntry bindGroupEntry = GetDefaultWGPUBindGroupEntry();
        bindGroupEntry.binding = m_BindGroupEntries.size();
        bindGroupEntry.sampler = sampler;

        m_BindGroupEntries.emplace_back(bindGroupEntry);

        WGPUSamplerBindingLayout samplerBindingLayout = {};
        samplerBindingLayout.nextInChain = nullptr;
        samplerBindingLayout.type = WGPUSamplerBindingType_Filtering;

        WGPUBindGroupLayoutEntry bindGroupLayoutEntry = GetDefaultWGPUBindGroupLayoutEntry();
        bindGroupLayoutEntry.nextInChain = nullptr;
        bindGroupLayoutEntry.binding = bindGroupEntry.binding;
        bindGroupLayoutEntry.visibility = m_Visibility;
        bindGroupLayoutEntry.sampler = samplerBindingLayout;

        m_BindGroupLayoutEntries.emplace_back(bindGroupLayoutEntry);
    }
}
