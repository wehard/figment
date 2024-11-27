#include "VulkanRenderPass.h"

namespace Figment
{
VulkanRenderPass::VulkanRenderPass(const VulkanContext& context,
                                   const VulkanRenderPass::RenderPassDescriptor&& descriptor):
    m_Context(context)
{
    // subpass: reference for subpass, cant link colorAttachment directly
    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment            = 0; // index
    colorAttachmentReference.layout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // converted to this during subpass (between
                                                  // initialLayout and finalLayout)

    // -- subpass --
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colorAttachmentReference;

    // determine when layout transitions occur using subpass dependencies
    VkSubpassDependency subpassDependencies[2];

    // transition: initialLayout > subpassLayout (VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    subpassDependencies[0].srcSubpass    = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].srcStageMask  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[0].dstSubpass    = 0; // index
    subpassDependencies[0].dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = 0;

    // transition: subpassLayout > finalLayout
    subpassDependencies[1].srcSubpass      = 0;
    subpassDependencies[1].srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].srcAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dstSubpass           = VK_SUBPASS_EXTERNAL; // index
    subpassDependencies[1].dstStageMask         = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[1].dstAccessMask        = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags      = 0;

    VkAttachmentDescription attachment          = {.format        = descriptor.ColorAttachment.Format,
                                                   .samples       = descriptor.ColorAttachment.Samples,
                                                   .loadOp        = descriptor.ColorAttachment.LoadOp,
                                                   .storeOp       = descriptor.ColorAttachment.StoreOp,
                                                   .stencilLoadOp = descriptor.ColorAttachment.StencilLoadOp,
                                                   .stencilStoreOp =
                                                       descriptor.ColorAttachment.StencilStoreOp,
                                                   .initialLayout = descriptor.ColorAttachment.InitialLayout,
                                                   .finalLayout   = descriptor.ColorAttachment.FinalLayout};
    // renderpass info
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount        = 1;
    renderPassCreateInfo.pAttachments           = &attachment;
    renderPassCreateInfo.subpassCount           = 1;
    renderPassCreateInfo.pSubpasses             = &subpass;
    renderPassCreateInfo.dependencyCount        = 2;
    renderPassCreateInfo.pDependencies          = subpassDependencies;

    checkVkResult(
        vkCreateRenderPass(m_Context.GetDevice(), &renderPassCreateInfo, nullptr, &m_RenderPass));
}

VulkanRenderPass::~VulkanRenderPass() {}
} // namespace Figment
