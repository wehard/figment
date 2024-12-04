#include "vk_imgui_presenter.h"
#include "utils.h"

namespace vk_imgui_presenter
{
VkImGuiPresenter::VkImGuiPresenter()
{
    if (!glfwInit())
    {
        throw std::runtime_error("GLFW initialization failed");
    }
}
VkImGuiPresenter::~VkImGuiPresenter()
{
    if (window)
    {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

static VkDescriptorPool createDescriptorPool(const VkDevice& device,
                                             std::vector<VkDescriptorPoolSize> poolSizes,
                                             uint32_t maxSets)
{
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();
    descriptorPoolCreateInfo.maxSets       = maxSets;

    VkDescriptorPool descriptorPool;
    checkVkResult(
        vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

    return descriptorPool;
}
void VkImGuiPresenter::init(const VkInstance& instance, const VkDevice& device,
                            const VkPhysicalDevice& physicalDevice, const VkQueue& queue,
                            const uint32_t queueIndex, const uint32_t width, const uint32_t height)
{
    if (!glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, queueIndex))
    {
        throw std::runtime_error("Physical device does not support presentation");
    }
    createWindow({width, height});
    createSurface(instance);
    createSwapchain(device, physicalDevice, {width, height});
    createSyncObjects(device);

    createCommandBuffers(device, queueIndex);

    vkGetSwapchainStatusKHR = reinterpret_cast<PFN_vkGetSwapchainStatusKHR>(
        vkGetDeviceProcAddr(device, "vkGetSwapchainStatusKHR"));

    std::vector<VkDescriptorPoolSize> poolSizes = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 256},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 256},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 256},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 256},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 256},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 256},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 256},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 256},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 256},
    };

    imguiRenderer = new ImGuiRenderer({
        .window          = window,
        .instance        = instance,
        .device          = device,
        .physicalDevice  = physicalDevice,
        .queue           = queue,
        .descriptorPool  = createDescriptorPool(device, poolSizes, 256),
        .minImageCount   = 2,
        .swapchainFormat = surfaceFormat.format,
    });
}

void VkImGuiPresenter::createCommandBuffers(const VkDevice& device,
                                            const uint32_t graphicsQueueIndex)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.queueFamilyIndex        = graphicsQueueIndex;
    commandPoolCreateInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    checkVkResult(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool        = commandPool;
    commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        checkVkResult(
            vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffers[i]));
    }
}
void VkImGuiPresenter::createWindow(const VkExtent2D extent)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(static_cast<int>(extent.width), static_cast<int>(extent.height),
                              "Vulkan Debug Presenter", nullptr, nullptr);
    if (!window)
    {
        throw std::runtime_error("GLFW window creation failed");
    }
    windowExtent = extent;
}
void VkImGuiPresenter::createSurface(const VkInstance& instance)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("GLFW surface creation failed");
    }

    // TODO: Get surface format and capabilities
}

static VkImageView createVkImageView(const VkDevice& device, const VkImage& image,
                                     const VkFormat format, const VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewCreateInfo           = {};
    viewCreateInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image                           = image;
    viewCreateInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.format                          = format;
    viewCreateInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

    viewCreateInfo.subresourceRange.aspectMask     = aspectFlags;
    viewCreateInfo.subresourceRange.baseMipLevel   = 0;
    viewCreateInfo.subresourceRange.levelCount     = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.layerCount     = 1;

    VkImageView imageView;
    checkVkResult(vkCreateImageView(device, &viewCreateInfo, nullptr, &imageView));
    return imageView;
}

void VkImGuiPresenter::createSwapchain(const VkDevice& device,
                                       const VkPhysicalDevice& physicalDevice,
                                       const VkExtent2D extent)
{
    uint32_t formatCount = 0;
    std::vector<VkSurfaceFormatKHR> surfaceFormats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount,
                                             surfaceFormats.data());
    }

    surfaceFormat                                = surfaceFormats[0];

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface                  = surface;
    swapchainCreateInfo.imageFormat              = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace          = surfaceFormat.colorSpace;
    swapchainCreateInfo.presentMode              = VK_PRESENT_MODE_IMMEDIATE_KHR;
    swapchainCreateInfo.imageExtent              = extent;
    swapchainCreateInfo.minImageCount            = 2;
    swapchainCreateInfo.imageArrayLayers         = 1;
    swapchainCreateInfo.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.preTransform             = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.clipped                  = VK_TRUE;

    checkVkResult(vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));

    checkVkResult(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr));
    images.resize(imageCount);
    checkVkResult(vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data()));

    imageViews.resize(imageCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        imageViews[i] =
            createVkImageView(device, images[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    // setObjectName(device, reinterpret_cast<uint64_t>(swapchain),
    // VK_OBJECT_TYPE_SWAPCHAIN_KHR,
    //               "Swapchain");
}
void VkImGuiPresenter::createSyncObjects(const VkDevice& device)
{
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    fences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fenceCreateInfo         = {};
    fenceCreateInfo.sType                     = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags                     = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {

        checkVkResult(
            vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]));
        // setObjectName(device, reinterpret_cast<uint64_t>(imageAvailableSemaphores[i]),
        //               VK_OBJECT_TYPE_SEMAPHORE, "Image Available Semaphore");
        checkVkResult(
            vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]));
        // setObjectName(device, reinterpret_cast<uint64_t>(renderFinishedSemaphores[i]),
        //               VK_OBJECT_TYPE_SEMAPHORE, "Render Finished Semaphore");

        vkCreateFence(device, &fenceCreateInfo, nullptr, &fences[i]);
    }
}
void VkImGuiPresenter::prepare(const VkDevice& device)
{
    vkWaitForFences(device, 1, &fences[frameIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
    vkResetFences(device, 1, &fences[frameIndex]);

    vkResetCommandBuffer(commandBuffers[frameIndex], 0);

    VkCommandBufferBeginInfo bufferBeginInfo = {};
    bufferBeginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(commandBuffers[frameIndex], &bufferBeginInfo);

    getNextImageIndex(device, imageAvailableSemaphores[frameIndex]);

    transitionImageLayout2(commandBuffers[frameIndex],
                           images[imageIndex],
                           VK_IMAGE_LAYOUT_UNDEFINED,
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    imguiRenderer->begin(commandBuffers[frameIndex],
                         {
                             .image      = images[imageIndex],
                             .imageView  = imageViews[imageIndex],
                             .renderArea = {0, 0, windowExtent.width, windowExtent.height},
                         });
}
void VkImGuiPresenter::present(const VkQueue& queue)
{
    imguiRenderer->end(commandBuffers[frameIndex]);
    transitionImageLayout2(commandBuffers[frameIndex],
                           images[imageIndex],
                           VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                           VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
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

    checkVkResult(vkQueueSubmit(queue, 1, &submitInfo, fences[frameIndex]));

    VkPresentInfoKHR presentInfo   = {};
    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &renderFinishedSemaphores[frameIndex];
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &swapchain;
    presentInfo.pImageIndices      = &imageIndex;

    checkVkResult(vkQueuePresentKHR(queue, &presentInfo));

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    glfwPollEvents();
}
void VkImGuiPresenter::cleanup(const VkInstance& instance, const VkDevice& device)
{
    for (size_t i = 0; i < imageCount; i++)
    {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, fences[i], nullptr);
    }
    vkFreeCommandBuffers(device, commandPool, MAX_FRAMES_IN_FLIGHT, commandBuffers.data());
    vkDestroyCommandPool(device, commandPool, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

} // namespace vk_imgui_presenter
