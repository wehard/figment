#pragma once

#include "utils.h"
#include <limits>
#include <vulkan/vulkan.h>

namespace figment::vulkan::debug
{
inline PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT     = nullptr;
inline PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT         = nullptr;
inline PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT   = nullptr;
inline PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT     = nullptr;
inline PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT = nullptr;
inline PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT     = nullptr;

inline void init(const VkInstance& instance)
{
    vkCmdBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
    vkCmdEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
    vkCmdInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
    vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
        vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
    vkQueueBeginDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkQueueBeginDebugUtilsLabelEXT"));
    vkQueueEndDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(
        vkGetInstanceProcAddr(instance, "vkQueueEndDebugUtilsLabelEXT"));
}

inline void beginLabel(const VkQueue& queue, const char* labelName)
{
    if (!vkQueueBeginDebugUtilsLabelEXT)
        return;

    VkDebugUtilsLabelEXT label = {};
    label.sType                = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
    label.pLabelName           = labelName;

    vkQueueBeginDebugUtilsLabelEXT(queue, &label);
}

inline void endLabel(const VkQueue& queue)
{
    if (vkQueueEndDebugUtilsLabelEXT)
    {
        vkQueueEndDebugUtilsLabelEXT(queue);
    }
}

inline void beginLabel(const VkCommandBuffer& commandBuffer, const char* labelName,
                       const std::array<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f})
{
    if (vkCmdBeginDebugUtilsLabelEXT)
    {
        VkDebugUtilsLabelEXT label = {};
        label.sType                = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        label.pLabelName           = labelName;
        label.color[0]             = color[0];
        label.color[1]             = color[1];
        label.color[2]             = color[2];
        label.color[3]             = color[3];
        vkCmdBeginDebugUtilsLabelEXT(commandBuffer, &label);
    }
}

inline void endLabel(const VkCommandBuffer& commandBuffer)
{
    if (vkCmdEndDebugUtilsLabelEXT)
    {
        vkCmdEndDebugUtilsLabelEXT(commandBuffer);
    }
}

inline void insertLabel(const VkCommandBuffer& commandBuffer, const char* labelName,
                        const std::array<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f})
{
    if (vkCmdInsertDebugUtilsLabelEXT)
    {
        VkDebugUtilsLabelEXT label = {};
        label.sType                = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        label.pLabelName           = labelName;
        label.color[0]             = color[0];
        label.color[1]             = color[1];
        label.color[2]             = color[2];
        label.color[3]             = color[3];
        vkCmdInsertDebugUtilsLabelEXT(commandBuffer, &label);
    }
}

inline void setObjectName(const VkDevice& device, const uint64_t objectHandle,
                          const VkObjectType objectType, const char* name)
{
    VkDebugUtilsObjectNameInfoEXT info = {};
    info.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    info.objectType                    = objectType;
    info.objectHandle                  = objectHandle;
    info.pObjectName                   = name;

    vkSetDebugUtilsObjectNameEXT(device, &info);
}

struct DebugScope
{
    DebugScope(const VkCommandBuffer& commandBuffer, const char* labelName,
               const std::array<float, 4> color = {0.0f, 0.0f, 0.0f, 1.0f}):
        commandBuffer(commandBuffer)
    {
        beginLabel(commandBuffer, labelName, color);
    }
    ~DebugScope() { endLabel(commandBuffer); }

    VkCommandBuffer commandBuffer;
};

#define VK_DEBUG_LABEL_SCOPE(commandBuffer, labelName)                                             \
    debug::DebugScope debugScope(labelName, commandBuffer);

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

struct Presenter
{
    Presenter()
    {
        if (!glfwInit())
        {
            throw VulkanError("GLFW initialization failed");
        }
    }

    void createWindow(const VkExtent2D extent)
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(static_cast<int>(extent.width), static_cast<int>(extent.height),
                                  "Vulkan Debug Presenter", nullptr, nullptr);
        if (!window)
        {
            throw VulkanError("GLFW window creation failed");
        }
        windowExtent = extent;
    }

    void createSurface(const VkInstance& instance)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw VulkanError("GLFW surface creation failed");
        }

        // TODO: Get surface format and capabilities
    }

    void createSwapchain(const VkDevice& device, const VkFormat imageFormat,
                         const VkColorSpaceKHR colorSpace, const VkPresentModeKHR presentMode,
                         const VkExtent2D extent)
    {
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface                  = surface;
        swapchainCreateInfo.imageFormat              = imageFormat;
        swapchainCreateInfo.imageColorSpace          = colorSpace;
        swapchainCreateInfo.presentMode              = presentMode;
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
                createVkImageView(device, images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        }

        setObjectName(device, reinterpret_cast<uint64_t>(swapchain), VK_OBJECT_TYPE_SWAPCHAIN_KHR,
                      "Swapchain");
    }

    [[nodiscard]] uint32_t getNextImageIndex(const VkDevice& device) const
    {
        uint32_t imageIndex = 0;
        checkVkResult(vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint64_t>::max(),
                                            VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex));
        return imageIndex;
    }

    void present(const VkQueue& queue, const VkDevice& device) const
    {
        const uint32_t imageIndex    = getNextImageIndex(device);
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pSwapchains      = &swapchain;
        presentInfo.swapchainCount   = 1;
        presentInfo.pImageIndices    = &imageIndex;
        presentInfo.pResults         = nullptr;

        vkQueuePresentKHR(queue, &presentInfo);

        glfwPollEvents();
    }

    static void addInstanceExtensions(std::vector<const char*>& instanceExtensions)
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (size_t i = 0; i < glfwExtensionCount; i++)
        {
            instanceExtensions.push_back(glfwExtensions[i]);
        }
    }

    GLFWwindow* window       = nullptr;
    VkExtent2D windowExtent  = {0, 0};
    VkSurfaceKHR surface     = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    uint32_t imageCount      = 0;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
};
} // namespace figment::vulkan::debug
