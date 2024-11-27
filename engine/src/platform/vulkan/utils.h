#pragma once

#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vulkan/vulkan.h>

namespace figment::vulkan
{
class VulkanError final: public std::runtime_error
{
public:
    explicit VulkanError(const std::string& msg):
        std::runtime_error(std::string("Vulkan error: ") + msg)
    {
    }
};

inline VkSampleCountFlagBits getVkSamplesCount(const int samples)
{
    switch (samples)
    {
        case 0:
        case 1:
            return VK_SAMPLE_COUNT_1_BIT;
        case 2:
            return VK_SAMPLE_COUNT_2_BIT;
        case 4:
            return VK_SAMPLE_COUNT_4_BIT;
        case 8:
            return VK_SAMPLE_COUNT_8_BIT;
        case 16:
            return VK_SAMPLE_COUNT_16_BIT;
        case 32:
            return VK_SAMPLE_COUNT_32_BIT;
        case 64:
            return VK_SAMPLE_COUNT_64_BIT;
        default:
            return VK_SAMPLE_COUNT_1_BIT;
    }
}

inline const char* vkResultToString(const VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:
            return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:
            return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED:
            return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
            return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_KHR:
            return "VK_ERROR_NOT_PERMITTED_KHR";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:
            return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:
            return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:
            return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:
            return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
            return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
            return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
            return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        default:
            return "Unhandled VkResult";
    }
}

inline void checkVkResult(const VkResult result)
{
    if (result == VK_SUCCESS)
        return;
    spdlog::error("Vulkan error: VkResult = {}", vkResultToString(result));
}

inline const char* vkSampleCountToString(const VkSampleCountFlagBits samples)
{
    switch (samples)
    {
        case VK_SAMPLE_COUNT_1_BIT:
            return "VK_SAMPLE_COUNT_1_BIT";
        case VK_SAMPLE_COUNT_2_BIT:
            return "VK_SAMPLE_COUNT_2_BIT";
        case VK_SAMPLE_COUNT_4_BIT:
            return "VK_SAMPLE_COUNT_4_BIT";
        case VK_SAMPLE_COUNT_8_BIT:
            return "VK_SAMPLE_COUNT_8_BIT";
        case VK_SAMPLE_COUNT_16_BIT:
            return "VK_SAMPLE_COUNT_16_BIT";
        case VK_SAMPLE_COUNT_32_BIT:
            return "VK_SAMPLE_COUNT_32_BIT";
        case VK_SAMPLE_COUNT_64_BIT:
            return "VK_SAMPLE_COUNT_64_BIT";
        default:
            return "VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM";
    }
}

inline uint32_t bytesPerPixelFromFormat(const VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_R8_UNORM:
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return 1;
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_R8G8B8A8_SRGB:
        case VK_FORMAT_R8G8B8A8_UNORM:
            return 4;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 16;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12;
        default:
            throw VulkanError("Unsupported format!");
    }
}

struct ImageFormatProperties
{
    VkImageType imageType   = VK_IMAGE_TYPE_1D;
    VkFormat format         = VK_FORMAT_UNDEFINED;
    VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
};

inline bool isFormatSupported(const VkPhysicalDevice& physicalDevice, const VkImageTiling tiling,
                              const ImageFormatProperties properties)
{
    VkImageFormatProperties imageFormatProperties;
    const VkResult result = vkGetPhysicalDeviceImageFormatProperties(
        physicalDevice, properties.format, properties.imageType, tiling, properties.usage, 0,
        &imageFormatProperties);
    return result == VK_SUCCESS;
}

inline void validateImageFormatProperties(const VkPhysicalDevice& physicalDevice,
                                          const VkImageTiling tiling,
                                          const ImageFormatProperties properties)
{
    if (!isFormatSupported(physicalDevice, tiling, properties))
    {
        // throw VulkanError(fmt::format("Unsupported image format properties: VkImageType={}, "
        //                               "VkFormat={}, VkImageUsageFlags={}",
        //                               properties.imageType, properties.format,
        //                               properties.usage));
        // spdlog::error("Unsupported image format properties: VkImageType={}, VkFormat={}, "
        //               "VkImageUsageFlags={}",
        //               properties.imageType, properties.format, properties.usage);
        spdlog::error("Unsupported image format properties");
    }
}

inline VkSampleCountFlagBits getMaxSampleCount(const VkPhysicalDevice& physicalDevice,
                                               ImageFormatProperties properties,
                                               VkSampleCountFlagBits maxSampleCount)
{
    VkImageFormatProperties imageFormatProperties;
    checkVkResult(vkGetPhysicalDeviceImageFormatProperties(
        physicalDevice, properties.format, properties.imageType, VK_IMAGE_TILING_OPTIMAL,
        properties.usage, 0, &imageFormatProperties));

    for (VkSampleCountFlagBits sampleCount = maxSampleCount; sampleCount >= VK_SAMPLE_COUNT_1_BIT;
         sampleCount                       = static_cast<VkSampleCountFlagBits>(sampleCount >> 1))
    {
        if (imageFormatProperties.sampleCounts & sampleCount)
        {
            maxSampleCount = sampleCount;
            break;
        }
    }
    return maxSampleCount;
}

} // namespace figment::vulkan
