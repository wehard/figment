#include "Renderer.h"

namespace Figment::Vulkan
{
    Renderer::Renderer(const VulkanContext &context) : m_Context(context)
    {
        m_OpaquePass = std::make_unique<VulkanRenderPass>(context, VulkanRenderPass::RenderPassDescriptor {
                .ColorAttachment = {
                        .Format = m_Context.SurfaceDetails().formats[0].format,
                        .Samples = VK_SAMPLE_COUNT_1_BIT,
                        .LoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                        .StoreOp = VK_ATTACHMENT_STORE_OP_STORE,
                        .StencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                        .StencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                        .InitialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                        .FinalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                }
        });
    }

    Renderer::~Renderer()
    {

    }
}
