#pragma once
#include "context.h"

namespace figment::vulkan
{
class VulkanRenderPass
{
public:
    struct AttachmentDescriptor
    {
        VkFormat Format                    = VK_FORMAT_UNDEFINED;
        VkSampleCountFlagBits Samples      = VK_SAMPLE_COUNT_1_BIT;
        VkAttachmentLoadOp LoadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        VkAttachmentStoreOp StoreOp        = VK_ATTACHMENT_STORE_OP_STORE;
        VkAttachmentLoadOp StencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        VkAttachmentStoreOp StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        VkImageLayout InitialLayout        = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout FinalLayout          = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    };

    struct RenderPassDescriptor
    {
        AttachmentDescriptor ColorAttachment = {};
    };

    VulkanRenderPass() = delete;
    VulkanRenderPass(const Context& context, const RenderPassDescriptor&& descriptor);
    ~VulkanRenderPass();

    [[nodiscard]] VkRenderPass Get() const { return m_RenderPass; }

private:
    const Context& m_Context;
    VkRenderPass m_RenderPass = VK_NULL_HANDLE;
};

} // namespace figment::vulkan
