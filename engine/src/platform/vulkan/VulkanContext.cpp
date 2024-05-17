#include "VulkanContext.h"
#include "Log.h"

#include <vector>

Figment::VulkanContext::~VulkanContext()
{

}

#define VK_CHECK_RESULT(f, m) { VkResult res = (f); if (res != VK_SUCCESS) { FIG_LOG_ERROR("Vulkan error: %s", m); } }

void Figment::VulkanContext::Init(uint32_t width, uint32_t height)
{
    // Create Vulkan instance
    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Figment";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "Figment";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char *> enabledExtensions = {
            "VK_KHR_surface",
            "VK_KHR_portability_enumeration"
    };

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
    const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = validationLayers;
    instanceCreateInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    // VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance), "Failed to create Vulkan instance");
    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
    if (result != VK_SUCCESS)
    {
        FIG_LOG_ERROR("Failed to create Vulkan instance");
    }
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

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.queueCreateInfoCount = 0;
    deviceCreateInfo.pQueueCreateInfos = nullptr;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = nullptr;
    deviceCreateInfo.pEnabledFeatures = nullptr;

    VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device), "Failed to create Vulkan device")
}
