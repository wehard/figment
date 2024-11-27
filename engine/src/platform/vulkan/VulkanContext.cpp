#include "VulkanContext.h"
#include "VulkanSwapchain.h"

#include <set>
#include <spdlog/spdlog.h>
#include <vector>

namespace figment
{
VulkanContext::~VulkanContext() {}

void VulkanContext::Init(uint32_t width, uint32_t height)
{
    spdlog::set_level(spdlog::level::debug);
    VkApplicationInfo applicationInfo  = {};
    applicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName   = "Figment";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName        = "Figment";
    applicationInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion         = VK_API_VERSION_1_0;

    createInstance(applicationInfo);
    createSurface();
    createDevice();
    createSwapchain();

    m_SingleTimeCommandPool = createCommandPool();

    if (glfwGetPhysicalDevicePresentationSupport(m_Instance, m_PhysicalDevice, 0))
        spdlog::info("Physical device supports presentation");
}

bool checkInstanceExtensionSupport(const std::vector<const char*>& requestedExtensions)
{
    uint32_t extensionCount;
    checkVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    checkVkResult(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                                         availableExtensions.data()));

    std::set<std::string> requiredExtensions(requestedExtensions.begin(),
                                             requestedExtensions.end());

    for (const auto& extension: availableExtensions)
    {
        if (requiredExtensions.contains(extension.extensionName))
        {
            spdlog::debug("Requested instance extension {} present", extension.extensionName);
        }
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool checkValidationLayerSupport(const std::vector<const char*>& requestedLayers)
{
    uint32_t layerCount = 0;
    checkVkResult(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
    std::vector<VkLayerProperties> availableLayers(layerCount);
    checkVkResult(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()));

    std::set<std::string> requiredLayers(requestedLayers.begin(), requestedLayers.end());

    for (const auto& layer: availableLayers)
    {
        if (requiredLayers.contains(layer.layerName))
        {
            spdlog::info("Requested layer {} present", layer.layerName);
        }
        requiredLayers.erase(layer.layerName);
    }

    return requiredLayers.empty();
}

void VulkanContext::createInstance(const VkApplicationInfo& applicationInfo)
{
    const auto getGlfwExtensions = [](std::vector<const char*>& instanceExtensions)
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (size_t i = 0; i < glfwExtensionCount; i++)
        {
            instanceExtensions.emplace_back(glfwExtensions[i]);
        }
    };

    auto instanceExtensions = std::vector<const char*>{};
    auto validationLayers   = std::vector<const char*>{};

    getGlfwExtensions(instanceExtensions);

#ifdef __APPLE__
    instanceExtensions.emplace_back("VK_KHR_portability_enumeration");
    instanceExtensions.emplace_back("VK_KHR_get_physical_device_properties2");
#endif

    instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    validationLayers.emplace_back("VK_LAYER_KHRONOS_validation");

    if (!checkInstanceExtensionSupport(instanceExtensions))
    {
        throw std::runtime_error("Instance extensions requested, but not available.");
    }

    if (!checkValidationLayerSupport(validationLayers))
    {
        throw std::runtime_error("Validation layers requested, but not available.");
    }

    VkInstanceCreateInfo instanceCreateInfo    = {};
    instanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext                   = nullptr;
    instanceCreateInfo.pApplicationInfo        = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(instanceExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
    instanceCreateInfo.enabledLayerCount       = static_cast<uint32_t>(validationLayers.size());
    instanceCreateInfo.ppEnabledLayerNames     = validationLayers.data();
#ifdef __APPLE__
    instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    checkVkResult(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance));

    spdlog::info("Vulkan instance created");
}

void VulkanContext::createSurface()
{
    checkVkResult(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface));
    spdlog::info("Vulkan surface created");
}
static std::vector<VkExtensionProperties> queryDeviceExtensions(VkPhysicalDevice physicalDevice)
{
    uint32_t extensionCount;
    checkVkResult(
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr));
    std::vector<VkExtensionProperties> extensions(extensionCount);
    checkVkResult(vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
                                                       extensions.data()));

    return extensions;
}

[[nodiscard]] bool checkDeviceExtensionsSupport(const VkPhysicalDevice& physicalDevice,
                                                const std::vector<const char*>& deviceExtensions)
{
    const auto availableExtensions = queryDeviceExtensions(physicalDevice);

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension: availableExtensions)
    {
        if (requiredExtensions.contains(extension.extensionName))
        {
            spdlog::debug("Requested device extension {} present", extension.extensionName);
        }
        requiredExtensions.erase(extension.extensionName);
    }

    for (const auto& extension: requiredExtensions)
    {
        spdlog::error("Required device extension {} not present", extension);
    }

    return requiredExtensions.empty();
}

static VkPhysicalDeviceProperties
getPhysicalDeviceProperties(const VkPhysicalDevice& physicalDevice)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    return properties;
}

void VulkanContext::createDevice()
{
    uint32_t deviceCount = 0;
    checkVkResult(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr));

    std::vector<VkPhysicalDevice> deviceList(deviceCount);
    checkVkResult(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, deviceList.data()));

    for (const auto& dev: deviceList)
    {
        VkPhysicalDeviceProperties properties = getPhysicalDeviceProperties(dev);
        spdlog::debug("Physical device found: {}", properties.deviceName);
    }

    for (const auto& dev: deviceList)
    {
        VkPhysicalDeviceProperties properties = getPhysicalDeviceProperties(dev);
        // if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        //         properties.vendorID == 0x10DE) // NVIDIA
        // {
        m_PhysicalDevice                      = dev;
        // m_PhysicalDeviceProperties = properties;
        spdlog::info("Selected physical device: {}", properties.deviceName);
        break;
        // }
    }

    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count,
                                             queueFamilyProperties.data());

    for (auto& queueFamilyProperty: queueFamilyProperties)
    {
        if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            break;
        m_GraphicsQueueIndex++;
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex        = m_GraphicsQueueIndex;
    queueCreateInfo.queueCount              = 1;
    constexpr float queuePriority           = 1.0f;
    queueCreateInfo.pQueuePriorities        = &queuePriority;

    // const auto requiredExtensions = std::vector {
    //         VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
    //         VK_EXT_ROBUSTNESS_2_EXTENSION_NAME, };
    const auto requiredExtensions = std::vector<const char*>{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef __APPLE__
                                                             "VK_KHR_portability_subset"
#endif
    };

    if (!checkDeviceExtensionsSupport(m_PhysicalDevice, requiredExtensions))
        throw std::runtime_error("Required device extensions are not supported!");

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature = {};
    dynamicRenderingFeature.sType =
        VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamicRenderingFeature.dynamicRendering                   = VK_TRUE;

    // VkPhysicalDeviceDescriptorBufferFeaturesEXT descriptorBufferFeatures = {};
    // descriptorBufferFeatures.sType =
    //         VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
    // descriptorBufferFeatures.descriptorBuffer                  = VK_TRUE;

    VkPhysicalDeviceRobustness2FeaturesEXT robustness2Features = {};
    robustness2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
    robustness2Features.nullDescriptor       = VK_TRUE;

    // Chain descriptorBufferFeatures to dynamicRenderingFeature
    dynamicRenderingFeature.pNext            = &robustness2Features;
    // Chain robustness2Features to descriptorBufferFeatures
    // descriptorBufferFeatures.pNext           = &robustness2Features;

    VkPhysicalDeviceFeatures deviceFeatures  = {};
    deviceFeatures.sampleRateShading         = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo      = {};
    deviceCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // deviceCreateInfo.pNext = &dynamicRenderingFeature;
    deviceCreateInfo.queueCreateInfoCount    = 1;
    deviceCreateInfo.pQueueCreateInfos       = &queueCreateInfo;
    deviceCreateInfo.enabledLayerCount       = 0;
    deviceCreateInfo.ppEnabledLayerNames     = nullptr;
    deviceCreateInfo.enabledExtensionCount   = requiredExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();
    deviceCreateInfo.pEnabledFeatures        = &deviceFeatures;
    checkVkResult(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device));

    spdlog::info("Vulkan logical device created");

    vkGetDeviceQueue(m_Device, m_GraphicsQueueIndex, 0, &m_GraphicsQueue);
}

static VulkanContext::VulkanSurfaceDetails getSurfaceDetails(VkPhysicalDevice device,
                                                             VkSurfaceKHR surface)
{
    VulkanContext::VulkanSurfaceDetails surfaceDetails;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceDetails.surfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        surfaceDetails.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                             surfaceDetails.formats.data());
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

void VulkanContext::createSwapchain()
{
    m_SurfaceDetails = getSurfaceDetails(m_PhysicalDevice, m_Surface);
    m_Swapchain      = new VulkanSwapchain(
        m_Device, {
                           .Surface           = m_Surface,
                           .SurfaceFormat     = m_SurfaceDetails.formats[0].format,
                           .SurfaceColorSpace = m_SurfaceDetails.formats[0].colorSpace,
                           .PresentMode       = m_SurfaceDetails.presentationModes[0],
                           .Extent            = m_SurfaceDetails.surfaceCapabilities.currentExtent,
                           .ImageCount        = m_SurfaceDetails.surfaceCapabilities.minImageCount + 1,
                           .Transform         = m_SurfaceDetails.surfaceCapabilities.currentTransform,
                  });

    spdlog::info("Vulkan swapchain created");

    m_DeletionQueue.Push(
        [this]()
        {
            // for (int i = 0; i < m_FrameData.ImageViews.size(); ++i)
            // {
            //     vkDestroyImageView(m_Device, m_FrameData.ImageViews[i], nullptr);
            // }
            // vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
        });
}

void VulkanContext::createPipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    pipelineCacheCreateInfo.pNext           = nullptr;
    pipelineCacheCreateInfo.flags           = 0;
    pipelineCacheCreateInfo.initialDataSize = 0;
    pipelineCacheCreateInfo.pInitialData    = nullptr;

    checkVkResult(
        vkCreatePipelineCache(m_Device, &pipelineCacheCreateInfo, nullptr, &m_PipelineCache));
}

VkDescriptorPool VulkanContext::createDescriptorPool(std::vector<VkDescriptorPoolSize> poolSizes,
                                                     uint32_t maxSets) const
{
    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes    = poolSizes.data();
    descriptorPoolCreateInfo.maxSets       = maxSets;

    VkDescriptorPool descriptorPool;
    checkVkResult(
        vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

    return descriptorPool;
}

VkCommandPool VulkanContext::createCommandPool() const
{
    VkCommandPool commandPool;
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex        = m_GraphicsQueueIndex;
    poolInfo.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    checkVkResult(vkCreateCommandPool(m_Device, &poolInfo, nullptr, &commandPool));
    return commandPool;
}

void VulkanContext::onResize(uint32_t width, uint32_t height)
{
    recreateSwapchain();
}

void VulkanContext::cleanupSwapchain() const
{
    vkDestroySwapchainKHR(m_Device, m_Swapchain->Get(), nullptr);
    delete m_Swapchain;
}

void VulkanContext::recreateSwapchain()
{
    vkDeviceWaitIdle(m_Device);

    cleanupSwapchain();
    createSwapchain();

    // vkResetCommandPool(m_Device, m_CommandPool, 0);
}

VkCommandBuffer VulkanContext::beginSingleTimeCommands() const
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool                 = m_SingleTimeCommandPool;
    allocInfo.commandBufferCount          = 1;

    VkCommandBuffer commandBuffer;
    checkVkResult(vkAllocateCommandBuffers(m_Device, &allocInfo, &commandBuffer));

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanContext::endSingleTimeCommands(VkCommandBuffer commandBuffer) const
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo       = {};
    submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &commandBuffer;

    checkVkResult(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
    vkQueueWaitIdle(m_GraphicsQueue);

    vkFreeCommandBuffers(m_Device, m_SingleTimeCommandPool, 1, &commandBuffer);
}

void VulkanContext::cleanup()
{
    // std::vector<VkFence> fences;
    // fences.reserve(m_SynchronizationObjects.size());
    // for (auto &synchronizationObject : m_SynchronizationObjects)
    // {
    //     fences.push_back(synchronizationObject.FenceDraw);
    // }
    // vkWaitForFences(m_Device, fences.size(), fences.data(), VK_TRUE,
    // std::numeric_limits<uint64_t>::max());
    m_DeletionQueue.Flush();
}

uint32_t VulkanContext::findMemoryTypeIndex(uint32_t allowedTypes,
                                            VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

    for (auto i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((allowedTypes & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }
    spdlog::error("Failed to find required memory type!");
    return (-1);
}

uint32_t VulkanContext::getSwapchainImageCount() const
{
    return m_Swapchain->GetImageCount();
}

std::vector<VkImageView> VulkanContext::getSwapchainImageViews() const
{
    return m_Swapchain->GetImageViews();
}

VkExtent2D VulkanContext::getSwapchainExtent() const
{
    return m_Swapchain->GetExtent();
}

VulkanSwapchain* VulkanContext::getSwapchain() const
{
    return m_Swapchain;
}
} // namespace figment
