#include "Window.h"
#include "VulkanContext.h"
#include "PerspectiveCamera.h"
#include "Log.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "VulkanBuffer.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/euler_angles.hpp"

using namespace Figment;

static glm::mat4 Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y),
            glm::radians(rotation.z));
    return matTranslate * matRotate * matScale;
}

static VkRenderPass CreateImGuiRenderPass(VulkanContext *vkContext)
{
    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = vkContext->SurfaceDetails().formats[0].format;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachment = {};
    colorAttachment.attachment = 0;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachment;

    VkSubpassDependency subpassDependency = {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependency.srcAccessMask = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &attachmentDescription;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &subpassDependency;
    VkRenderPass renderPass;
    if (vkCreateRenderPass(vkContext->GetDevice(), &info, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("Could not create Dear ImGui's render pass");
    }
    return renderPass;
}

int main()
{
    Log::Init();
    auto window = Figment::Window::Create("Figment", 1280, 720);
    auto vkContext = window->GetContext<VulkanContext>();



    PerspectiveCamera camera(1280.0 / 720.0);
    camera.SetPosition({ 0.0f, 0.0f, 2.0f });

    window->SetResizeEventCallback([&camera](WindowResizeEventData eventData)
    {
        camera.Resize((float)eventData.Width, (float)eventData.Height);
    });

    // auto renderPass = CreateImGuiRenderPass(vkContext.get());

    // auto context = ImGui::CreateContext();
    // ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window->GetNative(), true);
    // ImGui_ImplVulkan_InitInfo initInfo = {};
    // initInfo.UseDynamicRendering = false;
    // initInfo.Device = vkContext->GetDevice();
    // initInfo.Instance = vkContext->GetInstance();
    // initInfo.PhysicalDevice = vkContext->GetPhysicalDevice();
    // initInfo.Queue = vkContext->GetGraphicsQueue();
    // initInfo.QueueFamily = 0;
    // initInfo.PipelineCache = VK_NULL_HANDLE;
    // initInfo.DescriptorPool = vkContext->GetDescriptorPool();
    // initInfo.RenderPass = renderPass;
    // initInfo.Subpass = 0;
    // initInfo.MinImageCount = vkContext->SurfaceDetails().surfaceCapabilities.minImageCount;
    // initInfo.ImageCount = vkContext->SurfaceDetails().surfaceCapabilities.minImageCount + 1;
    // initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    // initInfo.Allocator = nullptr;
    // initInfo.CheckVkResultFn = nullptr;
    // ImGui_ImplVulkan_Init(&initInfo);
    // ImGui_ImplVulkan_CreateFontsTexture();
    std::vector<VulkanContext::Vertex> vertices = {
            {{ -0.5, -0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ -0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 }},
            {{ 0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ -0.5, -0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ 0.5, 0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ 0.5, -0.5, 0.0 }, { 0.0, 1.0, 0.0 }}};
    auto buffer = new VulkanBuffer(vkContext.get(), {
            .Data = vertices.data(),
            .ByteSize = vertices.size() * sizeof(VulkanContext::Vertex),
            .Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    });

    std::vector<VulkanContext::Vertex> vertices2 = {
            {{ 0.0, 0.5, 0.0 }, { 1.0, 1.0, 0.0 }},
            {{ -0.5, -0.5, 0.0 }, { 1.0, 1.0, 0.0 }},
            {{ 0.5, -0.5, 0.0 }, { 1.0, 1.0, 0.0 }}};

    auto zRotation = 0.0f;
    auto xPosition = 0.0f;

    while (!window->ShouldClose())
    {
        glfwPollEvents();
        camera.Update();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui_ImplVulkan_NewFrame();
        // ImGui::NewFrame();
        // ImGui::Begin("Hello, world!");
        // ImGui::Text("This is some useful text.");
        // ImGui::End();
        // ImGui::Render();
        // ImGui::EndFrame();

        zRotation += 1.0f;
        xPosition = sinf(glfwGetTime()) * 0.5f;
        vkContext->BeginFrame();
        vkContext->DebugDraw(*buffer, Transform(
                { xPosition, 0.0f, 0.0f },
                { 0.0f, 0.0f, zRotation },
                { 1.0f, 1.0f, 1.0f }),
                camera);
        // vkContext->DebugDraw(*buffer2);
        vkContext->EndFrame();
    }

    // ImGui_ImplVulkan_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext(context);
    return 0;
}
