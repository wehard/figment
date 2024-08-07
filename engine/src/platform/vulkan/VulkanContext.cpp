#include "VulkanContext.h"
#include "VulkanSwapchain.h"
#include "VulkanShader.h"

#include "glm/glm.hpp"

#include <vector>
#include <set>

namespace Figment
{
    Figment::VulkanContext::~VulkanContext()
    {
    }

    void Figment::VulkanContext::Init(uint32_t width, uint32_t height)
    {

        CreateInstance();
        CreateSurface();
        CreateDevice();
        CreateSwapchain();

        m_SingleTimeCommandPool = CreateCommandPool();

        if (glfwGetPhysicalDevicePresentationSupport(m_Instance, m_PhysicalDevice, 0))
        {
            FIG_LOG_INFO("Physical device supports presentation");
        }
    }

    static bool CheckInstanceExtensionSupport(std::vector<const char *> *checkExtensions)
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (const auto &checkExtension : *checkExtensions)
        {
            bool hasExtension = false;
            for (const auto &extension : extensions)
            {
                if (strcmp(checkExtension, extension.extensionName) == 0)
                {
                    hasExtension = true;
                    break;
                }
            }
            if (!hasExtension)
                return (false);
        }
        return (true);
    }

    void Figment::VulkanContext::CreateInstance()
    {
        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Figment";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pEngineName = "Figment";
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.apiVersion = VK_API_VERSION_1_0;

        // Get required instance extensions
        std::vector<const char *> instanceExtensions = std::vector<const char *>();
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (size_t i = 0; i < glfwExtensionCount; i++)
        {
            instanceExtensions.push_back(glfwExtensions[i]);
        }

        std::vector<const char *> enabledExtensions = {
                "VK_KHR_portability_enumeration",
                "VK_KHR_get_physical_device_properties2"
        };

        for (const auto &extension : enabledExtensions)
        {
            instanceExtensions.push_back(extension);
        }

        if (!CheckInstanceExtensionSupport(&instanceExtensions))
            throw std::runtime_error("VkInstance does not support required extensions!");

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
        const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
        instanceCreateInfo.enabledLayerCount = 1;
        instanceCreateInfo.ppEnabledLayerNames = validationLayers;
        instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        CheckVkResult(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance));
    }

    void Figment::VulkanContext::CreateSurface()
    {
        CheckVkResult(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface));
    }

    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device,
            const std::vector<const char *> &requiredExtensions)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        if (extensionCount == 0)
            return (false);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

        for (const auto deviceExtension : requiredExtensions)
        {
            bool hasExtension = false;
            for (const auto &extension : extensions)
            {
                if (strcmp(deviceExtension, extension.extensionName) == 0)
                {
                    hasExtension = true;
                    break;
                }
            }
            if (!hasExtension)
                return (false);
        }
        return (true);
    }

    void Figment::VulkanContext::CreateDevice()
    {
        // Create Vulkan device
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0)
        {
            FIG_LOG_ERROR("No Vulkan devices found");
        }

        std::vector<VkPhysicalDevice> deviceList(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, deviceList.data());

        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(deviceList[0], &deviceProperties);

        m_PhysicalDevice = deviceList[0];

        FIG_LOG_INFO("Vulkan physical device: %s", deviceProperties.deviceName);

        // Logical device

        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
        VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queues);
        for (uint32_t i = 0; i < count; i++)
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                m_GraphicsQueueIndex = i;
                break;
            }
        free(queues);

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_GraphicsQueueIndex;
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        auto supported = CheckDeviceExtensionSupport(m_PhysicalDevice, m_RequiredDeviceExtensions);

        // Create logical device
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = nullptr;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledLayerCount = 0;
        deviceCreateInfo.ppEnabledLayerNames = nullptr;
        deviceCreateInfo.enabledExtensionCount = m_RequiredDeviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = m_RequiredDeviceExtensions.data();
        VkPhysicalDeviceFeatures deviceFeatures = {}; // empty for now
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

        CheckVkResult(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device));

        vkGetDeviceQueue(m_Device, m_GraphicsQueueIndex, 0, &m_GraphicsQueue);
    }

    static VulkanContext::VulkanSurfaceDetails GetSurfaceDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        VulkanContext::VulkanSurfaceDetails surfaceDetails;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceDetails.surfaceCapabilities);

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            surfaceDetails.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, surfaceDetails.formats.data());
        }

        uint32_t presentationCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);
        if (presentationCount != 0)
        {
            surfaceDetails.presentationModes.resize(presentationCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount,
                    surfaceDetails.presentationModes.data());
        }
        return surfaceDetails;
    }

    void Figment::VulkanContext::CreateSwapchain()
    {
        m_SurfaceDetails = GetSurfaceDetails(m_PhysicalDevice, m_Surface);
        m_Swapchain = new VulkanSwapchain(m_Device, {
                .Surface = m_Surface,
                .SurfaceFormat = m_SurfaceDetails.formats[0].format,
                .SurfaceColorSpace = m_SurfaceDetails.formats[0].colorSpace,
                .PresentMode = m_SurfaceDetails.presentationModes[0],
                .Extent = m_SurfaceDetails.surfaceCapabilities.currentExtent,
                .ImageCount = m_SurfaceDetails.surfaceCapabilities.minImageCount + 1,
                .Transform = m_SurfaceDetails.surfaceCapabilities.currentTransform,
        });

        m_DeletionQueue.Push([this]()
        {
            // for (int i = 0; i < m_FrameData.ImageViews.size(); ++i)
            // {
            //     vkDestroyImageView(m_Device, m_FrameData.ImageViews[i], nullptr);
            // }
            // vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        });
    }

    void Figment::VulkanContext::CreatePipelineCache()
    {
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipelineCacheCreateInfo.pNext = nullptr;
        pipelineCacheCreateInfo.flags = 0;
        pipelineCacheCreateInfo.initialDataSize = 0;
        pipelineCacheCreateInfo.pInitialData = nullptr;

        CheckVkResult(vkCreatePipelineCache(m_Device, &pipelineCacheCreateInfo, nullptr, &m_PipelineCache));
    }

    VkDescriptorPool VulkanContext::CreateDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes,
            uint32_t maxSets) const
    {
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
        descriptorPoolCreateInfo.maxSets = maxSets;

        VkDescriptorPool descriptorPool;
        CheckVkResult(vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

        return descriptorPool;
    }

    VkCommandPool Figment::VulkanContext::CreateCommandPool() const
    {
        VkCommandPool commandPool;
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = m_GraphicsQueueIndex;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        CheckVkResult(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &commandPool));
        return commandPool;
    }

    void VulkanContext::OnResize(uint32_t width, uint32_t height)
    {
        RecreateSwapchain();
    }

    void VulkanContext::CleanupSwapchain()
    {
        vkDestroySwapchainKHR(m_Device, m_Swapchain->Get(), nullptr);
        delete m_Swapchain;
    }

    void VulkanContext::RecreateSwapchain()
    {
        vkDeviceWaitIdle(m_Device);

        CleanupSwapchain();
        CreateSwapchain();

        // vkResetCommandPool(m_Device, m_CommandPool, 0);
    }

    VkCommandBuffer VulkanContext::BeginSingleTimeCommands() const
    {
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_SingleTimeCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        CheckVkResult(vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void VulkanContext::EndSingleTimeCommands(VkCommandBuffer commandBuffer) const
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        CheckVkResult(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
        vkQueueWaitIdle(m_GraphicsQueue);

        vkFreeCommandBuffers(m_Device, m_SingleTimeCommandPool, 1, &commandBuffer);
    }

    void VulkanContext::Cleanup()
    {
        // std::vector<VkFence> fences;
        // fences.reserve(m_SynchronizationObjects.size());
        // for (auto &synchronizationObject : m_SynchronizationObjects)
        // {
        //     fences.push_back(synchronizationObject.FenceDraw);
        // }
        // vkWaitForFences(m_Device, fences.size(), fences.data(), VK_TRUE, std::numeric_limits<uint64_t>::max());
        m_DeletionQueue.Flush();
    }

    uint32_t VulkanContext::FindMemoryTypeIndex(uint32_t allowedTypes, VkMemoryPropertyFlags properties) const
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

        for (auto i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if ((allowedTypes & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        FIG_LOG_ERROR("Failed to find required memory type!");
        return (-1);
    }

    uint32_t VulkanContext::GetSwapchainImageCount() const
    {
        return m_Swapchain->GetImageCount();
    }

    std::vector<VkImageView> VulkanContext::GetSwapchainImageViews() const
    {
        return m_Swapchain->GetImageViews();
    }

    VkExtent2D VulkanContext::GetSwapchainExtent() const
    {
        return m_Swapchain->GetExtent();
    }

    VulkanSwapchain *VulkanContext::GetSwapchain() const
    {
        return m_Swapchain;
    }
}
