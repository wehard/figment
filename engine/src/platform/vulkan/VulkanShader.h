#pragma once

#include "VulkanContext.h"
#include <string>

namespace figment
{
class VulkanShader
{
public:
    VulkanShader(const VulkanContext& vkContext, const std::string& vertexPath,
                 const std::string& fragmentPath);
    ~VulkanShader();

    [[nodiscard]] VkShaderModule GetVertexModule() const { return m_VertexModule; }
    [[nodiscard]] VkShaderModule GetFragmentModule() const { return m_FragmentModule; }

private:
    const VulkanContext& m_Context;
    VkShaderModule m_VertexModule;
    VkShaderModule m_FragmentModule;
};
} // namespace figment
