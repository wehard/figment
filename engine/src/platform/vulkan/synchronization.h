#pragma once

#include "utils.h"
#include "vulkan/vulkan.h"

namespace figment::vulkan
{
VkSemaphore createSemaphore(const VkDevice& device)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    checkVkResult(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore));
    return semaphore;
}
} // namespace figment::vulkan
