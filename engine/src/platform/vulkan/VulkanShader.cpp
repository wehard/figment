#include "VulkanShader.h"

#include <fstream>

namespace Figment
{
static std::vector<char> ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");
    std::streamsize fileSize = file.tellg();
    std::vector<char> fileBuffer(fileSize);
    file.seekg(0); // move read pos to the start of file
    file.read(fileBuffer.data(), fileSize);
    file.close();
    return (fileBuffer);
}

static VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize                 = code.size();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    checkVkResult(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));
    return (shaderModule);
}

VulkanShader::VulkanShader(const VulkanContext& vkContext, const std::string& vertexPath,
                           const std::string& fragmentPath):
    m_Context(vkContext)
{
    auto vertexShaderCode   = ReadFile(vertexPath);
    auto fragmentShaderCode = ReadFile(fragmentPath);

    m_VertexModule          = CreateShaderModule(vkContext.GetDevice(), vertexShaderCode);
    m_FragmentModule        = CreateShaderModule(vkContext.GetDevice(), fragmentShaderCode);
}

VulkanShader::~VulkanShader()
{
    vkDestroyShaderModule(m_Context.GetDevice(), m_VertexModule, nullptr);
    vkDestroyShaderModule(m_Context.GetDevice(), m_FragmentModule, nullptr);
}
} // namespace Figment
