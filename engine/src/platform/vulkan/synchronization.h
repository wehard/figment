#pragma once

#include "debug_tools.h"
#include "utils.h"
#include "vulkan/vulkan.h"

namespace figment::vulkan
{
inline VkSemaphore createSemaphore(const VkDevice& device, const char* debugName = nullptr)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    checkVkResult(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphore));
    debug::setObjectName(device, reinterpret_cast<uint64_t>(semaphore), VK_OBJECT_TYPE_SEMAPHORE,
                         debugName);
    return semaphore;
}

inline VkFence createFence(const VkDevice& device, const bool signaled = false,
                           const char* debugName = nullptr)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags             = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VkFence fence;
    checkVkResult(vkCreateFence(device, &fenceCreateInfo, nullptr, &fence));
    debug::setObjectName(device, reinterpret_cast<uint64_t>(fence), VK_OBJECT_TYPE_FENCE,
                         debugName);
    return fence;
}
} // namespace figment::vulkan
