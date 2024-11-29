
#include "renderer.h"

#include <swapchain.h>
#include <synchronization.h>

namespace figment::vulkan
{
static VkCommandBuffer createCommandBuffer(const VkDevice& device, const VkCommandPool& commandPool)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool        = commandPool;
    commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer                = VK_NULL_HANDLE;
    checkVkResult(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer));
    return commandBuffer;
}

Renderer::Renderer(const Context& context): context(context)
{
    commandPool = context.createCommandPool();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        renderFinishedSemaphores.emplace_back(createSemaphore(
            context.GetDevice(), fmt::format("renderFinishedSemaphore {}", i).c_str()));
        imageAvailableSemaphores.emplace_back(createSemaphore(
            context.GetDevice(), fmt::format("imageAvailableSemaphore {}", i).c_str()));
        fences.emplace_back(
            createFence(context.GetDevice(), true, fmt::format("fence {}", i).c_str()));
        commandBuffers.emplace_back(createCommandBuffer(context.GetDevice(), commandPool));
    }
}
Renderer::~Renderer() {}

Renderer::Data Renderer::begin(const Swapchain& swapchain)
{
    debug::beginLabel(context.GetGraphicsQueue(), "Renderer::begin");

    vkWaitForFences(context.GetDevice(), 1, &fences[frameIndex], VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    vkResetFences(context.GetDevice(), 1, &fences[frameIndex]);

    vkResetCommandBuffer(commandBuffers[frameIndex], 0);

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commandBuffers[frameIndex], &bufferBeginInfo);

    imageIndex = swapchainNextImageIndex(context.GetDevice(), swapchain.swapchain,
                                         imageAvailableSemaphore());

    return {commandBuffers[frameIndex], swapchain.images[imageIndex],
            swapchain.imageViews[imageIndex]};
}

void Renderer::render()
{
    // TODO: Implement
}

void Renderer::end(const Swapchain& swapchain)
{
    checkVkResult(vkEndCommandBuffer(commandBuffers[frameIndex]));

    constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSemaphore waitSemaphores[]          = {imageAvailableSemaphores[frameIndex]};
    const VkSemaphore signalSemaphores[]        = {renderFinishedSemaphores[frameIndex]};

    VkSubmitInfo submitInfo                     = {};
    submitInfo.sType                            = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount               = 1;
    submitInfo.pWaitSemaphores                  = waitSemaphores;
    submitInfo.pWaitDstStageMask                = waitStages;
    submitInfo.commandBufferCount               = 1;
    submitInfo.pCommandBuffers                  = &commandBuffers[frameIndex];
    submitInfo.signalSemaphoreCount             = 1;
    submitInfo.pSignalSemaphores                = signalSemaphores;

    debug::endLabel(context.GetGraphicsQueue());
    checkVkResult(vkQueueSubmit(context.GetGraphicsQueue(), 1, &submitInfo, fences[frameIndex]));

    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &renderFinishedSemaphores[frameIndex];
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain.swapchain;
    presentInfo.pImageIndices      = &imageIndex;

    checkVkResult(vkQueuePresentKHR(context.GetGraphicsQueue(), &presentInfo));

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}
} // namespace figment::vulkan
