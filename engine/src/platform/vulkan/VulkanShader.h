#pragma once

#include "context.h"
#include <string>

namespace figment::vulkan
{
class VulkanShader
{
public:
    VulkanShader(const Context& vkContext, const std::string& vertexPath,
                 const std::string& fragmentPath);
    ~VulkanShader();

    [[nodiscard]] VkShaderModule GetVertexModule() const { return m_VertexModule; }
    [[nodiscard]] VkShaderModule GetFragmentModule() const { return m_FragmentModule; }

private:
    const Context& m_Context;
    VkShaderModule m_VertexModule;
    VkShaderModule m_FragmentModule;
};
} // namespace figment::vulkan
