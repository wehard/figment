#include "VulkanContext.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"

#include "glm/glm.hpp"

#include <vector>
#include <set>

namespace Figment
{
#define VK_CHECK_RESULT(f, s, e) { VkResult res = (f); if (res != VK_SUCCESS) { FIG_LOG_ERROR("Vulkan error: %s", e); } else { FIG_LOG_INFO("Vulkan: %s", s); } }

    Figment::VulkanContext::~VulkanContext()
    {
    }

    void Figment::VulkanContext::Init(uint32_t width, uint32_t height)
    {

        CreateInstance();
        CreateSurface();
        CreateDevice();
        CreateSwapChain();
        CreateRenderPass();

        std::vector<Vertex> vertices = {
                {{ 0.0, -0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
                {{ 0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 }},
                {{ -0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 }}};
        m_Buffer = new VulkanBuffer(this, {
                .Data = vertices.data(),
                .ByteSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex)),
                .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        });

        m_Shader = new VulkanShader(*this, "res/shader.vert.spv", "res/shader.frag.spv");

        UniformBufferObject ubo = {
                .Model = glm::mat4(1.0f),
                .View = glm::mat4(1.0f),
                .Projection = glm::mat4(1.0f)
        };

        for (int i = 0; i < MAX_FRAME_DRAWS; i++)
        {
            m_UniformBuffers.push_back(new VulkanBuffer(this, {
                    .Name = "UniformBuffer",
                    .Data = &ubo,
                    .ByteSize = sizeof(UniformBufferObject),
                    .Usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    .MemoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            }));
        }

        CreatePipeline(m_Shader->GetVertexModule(), m_Shader->GetFragmentModule());
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateDescriptorPool();
        CreateDescriptorSets();
        CreateSynchronization();

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

        VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance), "Instance created",
                "Failed to create instance");
    }

    void Figment::VulkanContext::CreateSurface()
    {
        VK_CHECK_RESULT(glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface), "Vulkan surface created",
                "Failed to create Vulkan surface");
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

        VK_CHECK_RESULT(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device),
                "Logical device created",
                "Failed to create logical device")

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

    static VkImageView CreateVkImageView(VkDevice device, VkImage image, VkFormat format,
            VkImageAspectFlags aspectFlags)
    {
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.format = format;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        viewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView imageView;
        VkResult result = vkCreateImageView(device, &viewCreateInfo, nullptr, &imageView);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create an Image View!");
        return imageView;
    }

    void Figment::VulkanContext::CreateSwapChain()
    {
        m_SurfaceDetails = GetSurfaceDetails(m_PhysicalDevice, m_Surface);
        VkSurfaceFormatKHR surfaceFormat = m_SurfaceDetails.formats[0];
        VkPresentModeKHR presentMode = m_SurfaceDetails.presentationModes[0];
        VkExtent2D extent = m_SurfaceDetails.surfaceCapabilities.currentExtent;
        uint32_t imageCount = m_SurfaceDetails.surfaceCapabilities.minImageCount + 1;

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
        swapChainCreateInfo.preTransform = m_SurfaceDetails.surfaceCapabilities.currentTransform;
        swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapChainCreateInfo.clipped = VK_TRUE;

        CheckVkResult(vkCreateSwapchainKHR(m_Device, &swapChainCreateInfo, nullptr, &m_SwapChain));

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;

        uint32_t swapChainImageCount = 0;
        CheckVkResult(vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &swapChainImageCount, nullptr));
        std::vector<VkImage> images(swapChainImageCount);
        CheckVkResult(vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &swapChainImageCount, images.data()));

        m_FrameData.Init(swapChainImageCount);
        for (size_t i = 0; i < swapChainImageCount; i++)
        {
            m_FrameData.Images[i] = images[i];
            m_FrameData.ImageViews[i] = CreateVkImageView(m_Device, images[i], m_SwapChainImageFormat,
                    VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void Figment::VulkanContext::CreateRenderPass()
    {
        // color attachment of render pass
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_SwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

        // framebuffer data will be stored as image but images can be given different data layouts
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // subpass: reference for subpass, cant link colorAttachment directly
        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;                                    // index
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // converted to this during subpass (between initialLayout and finalLayout)

        // -- subpass --
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        // determine when layout transitions occur using subpass dependencies
        VkSubpassDependency subpassDependencies[2];

        // transition: initialLayout > subpassLayout (VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
        subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        subpassDependencies[0].dstSubpass = 0; //index
        subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependencies[0].dstAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpassDependencies[0].dependencyFlags = 0;

        // transition: subpassLayout > finalLayout
        subpassDependencies[1].srcSubpass = 0;
        subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        subpassDependencies[1].srcAccessMask =
                VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; //index
        subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        subpassDependencies[1].dependencyFlags = 0;

        // renderpass info
        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 2;
        renderPassCreateInfo.pDependencies = subpassDependencies;

        VK_CHECK_RESULT(vkCreateRenderPass(m_Device, &renderPassCreateInfo, nullptr, &m_RenderPass),
                "Render pass created",
                "Failed to create render pass");
    }

    void Figment::VulkanContext::CreatePipeline(VkShaderModule vertexModule, VkShaderModule fragmentModule)
    {
        m_Pipeline = new VulkanPipeline(*this, {
                .ViewportWidth = m_SwapChainExtent.width,
                .ViewportHeight = m_SwapChainExtent.height,
                .VertexInput = {
                        .Binding = 0,
                        .Stride = sizeof(Vertex),
                        .InputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                        .Attributes = {
                                {
                                        .location = 0,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                                        .offset = offsetof(Vertex, Position)
                                },
                                {
                                        .location = 1,
                                        .binding = 0,
                                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                                        .offset = offsetof(Vertex, Color)
                                }
                        }
                },
                .RenderPass = m_RenderPass,
                .VertexModule = vertexModule,
                .FragmentModule = fragmentModule,
                .DescriptorSetLayoutBindings = {
                        {
                                .binding = 0,
                                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                .descriptorCount = 1,
                                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                                .pImmutableSamplers = nullptr
                        }
                }
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

        VK_CHECK_RESULT(vkCreatePipelineCache(m_Device, &pipelineCacheCreateInfo, nullptr, &m_PipelineCache),
                "PipelineCache created", "Failed to create PipelineCache!")
    }

    void Figment::VulkanContext::CreateDescriptorPool()
    {
        VkDescriptorPoolSize poolSize = {
                .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = static_cast<uint32_t>(MAX_FRAME_DRAWS)
        };
        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.flags = 0;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &poolSize;
        descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAME_DRAWS);

        vkCreateDescriptorPool(m_Device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool);
    }

    void Figment::VulkanContext::CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAME_DRAWS, *m_Pipeline->GetDescriptorSetLayout());
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = layouts.size();
        descriptorSetAllocateInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(MAX_FRAME_DRAWS);
        CheckVkResult(vkAllocateDescriptorSets(m_Device, &descriptorSetAllocateInfo, m_DescriptorSets.data()));

        for (int i = 0; i < MAX_FRAME_DRAWS; i++)
        {
            VkDescriptorBufferInfo bufferInfo {};
            bufferInfo.buffer = m_UniformBuffers[i]->Get();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkWriteDescriptorSet descriptorWrite = {};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
        }

    }

    void Figment::VulkanContext::CreateCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = m_GraphicsQueueIndex;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult result = vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to create command pool!");
    }

    void Figment::VulkanContext::CreateFramebuffers()
    {
        m_FrameData.Framebuffers.resize(m_FrameData.ImageViews.size());
        for (size_t i = 0; i < m_FrameData.Framebuffers.size(); i++)
        {
            std::array<VkImageView, 1> attachments = {
                    m_FrameData.ImageViews[i] };

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = m_RenderPass;
            framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferCreateInfo.pAttachments = attachments.data();
            framebufferCreateInfo.width = m_SwapChainExtent.width;
            framebufferCreateInfo.height = m_SwapChainExtent.height;
            framebufferCreateInfo.layers = 1;

            VkResult result = vkCreateFramebuffer(m_Device, &framebufferCreateInfo, nullptr,
                    &m_FrameData.Framebuffers[i]);
            if (result != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer!");
        }
    }

    void Figment::VulkanContext::CreateCommandBuffers()
    {
        m_FrameData.CommandBuffers.resize(MAX_FRAME_DRAWS);
        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = m_CommandPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // can only be executed by queue, not like secondary that can be executed by primary command buffers
        commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(m_FrameData.CommandBuffers.size());

        VkResult result = vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo,
                &m_FrameData.CommandBuffers[0]);
        if (result != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers!");
    }

    void Figment::VulkanContext::CreateSynchronization()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (auto &m_SynchronizationObject : m_SynchronizationObjects)
        {
            if (vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr,
                    &m_SynchronizationObject.SemaphoreImageAvailable) != VK_SUCCESS
                    ||
                            vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr,
                                    &m_SynchronizationObject.SemaphoreRenderFinished)
                                    != VK_SUCCESS ||
                    vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_SynchronizationObject.FenceDraw)
                            != VK_SUCCESS)
                throw std::runtime_error("Failed to create semaphore or fence!");
        }
    }

    void VulkanContext::BeginFrame()
    {
        CheckVkResult(vkWaitForFences(m_Device, 1, &m_SynchronizationObjects[m_FrameIndex].FenceDraw, VK_TRUE,
                std::numeric_limits<uint64_t>::max()));

        CheckVkResult(vkResetFences(m_Device, 1, &m_SynchronizationObjects[m_FrameIndex].FenceDraw));

        // get next available image to draw to and set semaphore to signal when it's ready to be drawn to
        CheckVkResult(vkAcquireNextImageKHR(m_Device, m_SwapChain, std::numeric_limits<uint64_t>::max(),
                m_SynchronizationObjects[m_FrameIndex].SemaphoreImageAvailable, VK_NULL_HANDLE, &m_ImageIndex));

        VkCommandBufferBeginInfo bufferBeginInfo = {};
        bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = m_RenderPass;
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = m_SwapChainExtent;
        VkClearValue clearValues[] = {
                { 0.1f, 0.1f, 0.1f, 1.0f }};
        renderPassBeginInfo.pClearValues = clearValues;
        renderPassBeginInfo.clearValueCount = 1;

        renderPassBeginInfo.framebuffer = m_FrameData.Framebuffers[m_ImageIndex];

        CheckVkResult(vkBeginCommandBuffer(m_FrameData.CommandBuffers[m_FrameIndex], &bufferBeginInfo));

        // begin render pass
        vkCmdBeginRenderPass(m_FrameData.CommandBuffers[m_FrameIndex], &renderPassBeginInfo,
                VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanContext::EndFrame()
    {
        vkCmdEndRenderPass(m_FrameData.CommandBuffers[m_FrameIndex]);

        CheckVkResult(vkEndCommandBuffer(m_FrameData.CommandBuffers[m_FrameIndex]));

        // submit command buffer to queue for execution
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_SynchronizationObjects[m_FrameIndex].SemaphoreImageAvailable;
        VkPipelineStageFlags waitStages[] = {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_FrameData.CommandBuffers[m_FrameIndex];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_SynchronizationObjects[m_FrameIndex].SemaphoreRenderFinished;

        CheckVkResult(
                vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_SynchronizationObjects[m_FrameIndex].FenceDraw));

        // present image to screen when finished rendering
        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_SynchronizationObjects[m_FrameIndex].SemaphoreRenderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_SwapChain;
        presentInfo.pImageIndices = &m_ImageIndex;

        CheckVkResult(vkQueuePresentKHR(m_GraphicsQueue, &presentInfo));

        m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAME_DRAWS;
    }

    void VulkanContext::DebugDraw(VulkanBuffer &buffer, glm::mat4 transform, Camera &camera)
    {
        vkCmdBindPipeline(m_FrameData.CommandBuffers[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->Get());

        UniformBufferObject ubo = {
                .Model = transform,
                .View = camera.GetViewMatrix(),
                .Projection = camera.GetProjectionMatrix(),
        };
        // m_UBO.Projection[1][1] *= -1;
        m_UniformBuffers[m_FrameIndex]->SetData(&ubo, sizeof(UniformBufferObject));

        VkBuffer buffers[] = { buffer.Get() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(m_FrameData.CommandBuffers[m_FrameIndex], 0, 1, buffers, offsets);

        vkCmdBindDescriptorSets(m_FrameData.CommandBuffers[m_FrameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS,
                m_Pipeline->GetLayout(),
                0, 1, &m_DescriptorSets[m_FrameIndex], 0, nullptr);
        vkCmdDraw(m_FrameData.CommandBuffers[m_FrameIndex], 6, 1, 0, 0);
    }

    void VulkanContext::OnResize(uint32_t width, uint32_t height)
    {
        RecreateSwapChain();
    }

    void VulkanContext::CleanupSwapChain()
    {
        for (auto framebuffer : m_FrameData.Framebuffers)
        {
            vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
        }
        m_FrameData.Framebuffers.clear();

        for (auto imageView : m_FrameData.ImageViews)
        {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }
        m_FrameData.ImageViews.clear();

        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);

        delete m_Pipeline;
    }

    void VulkanContext::RecreateSwapChain()
    {
        vkDeviceWaitIdle(m_Device);

        CleanupSwapChain();
        CreateSwapChain();
        CreateFramebuffers();
        CreatePipeline(m_Shader->GetVertexModule(), m_Shader->GetFragmentModule());

        vkResetCommandPool(m_Device, m_CommandPool, 0);
    }
}
