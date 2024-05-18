#include "VulkanContext.h"
#include "Log.h"

#include <vector>
#include <GLFW/glfw3.h>

Figment::VulkanContext::~VulkanContext()
{

}

#define VK_CHECK_RESULT(f, s, e) { VkResult res = (f); if (res != VK_SUCCESS) { FIG_LOG_ERROR("Vulkan error: %s", e); } else { FIG_LOG_INFO("Vulkan: %s", s); } }

void Figment::VulkanContext::Init(uint32_t width, uint32_t height)
{
    CreateInstance();
    CreateSurface();
    CreateDevice();
    CreateSwapChain();
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
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);


    std::vector<const char *> enabledExtensions = {
            "VK_KHR_surface",
            "VK_KHR_portability_enumeration",
            "VK_KHR_get_physical_device_properties2"
    };

    enabledExtensions.insert(enabledExtensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = validationLayers;
    instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance), "Instance created", "Failed to create instance");
}

void Figment::VulkanContext::CreateSurface()
{
    VK_CHECK_RESULT(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface), "Vulkan surface created", "Failed to create Vulkan surface");
}

static void GetQueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface, uint32_t &graphicsFamily, uint32_t &presentFamily)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    graphicsFamily = UINT32_MAX;
    presentFamily = UINT32_MAX;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsFamily = i;
        }

        if (presentSupport)
        {
            presentFamily = i;
        }

        if (graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX)
        {
            break;
        }
    }
}

static bool CheckDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char *> &requiredExtensions)
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
    // VK_KHR_portability_subset device extension

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

    uint32_t graphicsFamily, presentFamily;
    GetQueueFamilyIndices(m_PhysicalDevice, m_Surface, graphicsFamily, presentFamily);

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = graphicsFamily;
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

    VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device), "Logical device created", "Failed to create logical device")

    vkGetDeviceQueue(m_Device, graphicsFamily, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, presentFamily, 0, &m_PresentationQueue);
}

static Figment::SurfaceDetails GetSurfaceDetails(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    Figment::SurfaceDetails surfaceDetails;

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
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, surfaceDetails.presentationModes.data());
    }
    return surfaceDetails;
}

void Figment::VulkanContext::CreateSwapChain()
{
    SurfaceDetails surfaceDetails = GetSurfaceDetails(m_PhysicalDevice, m_Surface);
    VkSurfaceFormatKHR surfaceFormat = surfaceDetails.formats[0];
    VkPresentModeKHR presentMode = surfaceDetails.presentationModes[0];
    VkExtent2D extent = surfaceDetails.surfaceCapabilities.currentExtent;

    uint32_t imageCount = surfaceDetails.surfaceCapabilities.minImageCount + 1;

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = m_Surface;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapChainCreateInfo.preTransform = surfaceDetails.surfaceCapabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.clipped = VK_TRUE;

    VkResult result = vkCreateSwapchainKHR(m_Device, &swapChainCreateInfo, nullptr, &m_SwapChain);
    if (result != VK_SUCCESS)
    {
        FIG_LOG_ERROR("Failed to create swap chain");
    }
    else
    {
        FIG_LOG_INFO("Swap chain created");
    }
}
