#include "Window.h"
#include "VulkanContext.h"
#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
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
#include "Image.h"
#include "Input.h"
#include "Renderer.h"

using namespace Figment;

static glm::mat4 Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f), scale);
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0), position);
    glm::mat4 matRotate = glm::eulerAngleXYZ(glm::radians(rotation.x), glm::radians(rotation.y),
            glm::radians(rotation.z));
    return matTranslate * matRotate * matScale;
}

static ImGuiContext *ImGuiInit(VulkanContext *vulkanContext, Vulkan::Renderer *renderer, GLFWwindow *window)
{
    auto context = ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.UseDynamicRendering = false;
    initInfo.Device = vulkanContext->GetDevice();
    initInfo.Instance = vulkanContext->GetInstance();
    initInfo.PhysicalDevice = vulkanContext->GetPhysicalDevice();
    initInfo.Queue = vulkanContext->GetGraphicsQueue();
    initInfo.QueueFamily = 0;
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = vulkanContext->CreateDescriptorPool({
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    }, 1000);
    initInfo.RenderPass = renderer->GetGuiRenderPass();
    initInfo.Subpass = 0;
    initInfo.MinImageCount = vulkanContext->SurfaceDetails().surfaceCapabilities.minImageCount;
    initInfo.ImageCount = vulkanContext->SurfaceDetails().surfaceCapabilities.minImageCount + 1;
    initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = CheckVkResult;
    ImGui_ImplVulkan_Init(&initInfo);

    auto cb = vulkanContext->BeginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture();
    vulkanContext->EndSingleTimeCommands(cb);

    return context;
}

static void ImGuiShutdown(ImGuiContext *context)
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context);
}

int main()
{
    Log::Init();
    auto window = Window::Create("Figment", 1280, 720);
    Input::Initialize((GLFWwindow *)window->GetNative());
    auto vulkanContext = window->GetContext<VulkanContext>();
    auto renderer = Vulkan::Renderer(*vulkanContext);

    PerspectiveCamera camera(1280.0 / 720.0);
    camera.SetPosition({ 0.0f, 0.0f, 2.0f });

    window->SetResizeEventCallback([&camera, &renderer](WindowResizeEventData eventData)
    {
        camera.Resize((float)eventData.Width, (float)eventData.Height);
        renderer.OnResize(eventData.Width, eventData.Height);
    });

    auto imGuiContext = ImGuiInit(vulkanContext.get(), &renderer, (GLFWwindow *)window->GetNative());

    Image image = Image::Load("res/texture.png");

    auto texture = VulkanTexture(*vulkanContext, {
            .Width = static_cast<int>(image.GetWidth()),
            .Height = static_cast<int>(image.GetHeight()),
            .Channels = 4,
            .Data = image.GetData(),
            .Format = VK_FORMAT_R8G8B8A8_UNORM
    });

    std::vector<VulkanContext::Vertex> vertices = {
            {{ -0.5, -0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ -0.5, 0.5, 0.0 }, { 0.0, 1.0, 0.0 }},
            {{ 0.5, 0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ -0.5, -0.5, 0.0 }, { 0.0, 0.0, 1.0 }},
            {{ 0.5, 0.5, 0.0 }, { 1.0, 0.0, 0.0 }},
            {{ 0.5, -0.5, 0.0 }, { 0.0, 1.0, 0.0 }}};
    auto buffer = new VulkanBuffer(*vulkanContext, {
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

    auto id = ImGui_ImplVulkan_AddTexture(texture.GetSampler(), texture.GetImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    while (!window->ShouldClose() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
    {
        glfwPollEvents();
        Input::Update();
        camera.Update();

        zRotation += 1.0f;
        xPosition = sinf((float)glfwGetTime() * 2.0f); // * 0.5f;
        renderer.BeginFrame();
        renderer.Begin(camera);
        renderer.Draw(*buffer, Transform(
                        { xPosition, 0.0f, 0.0f },
                        { 0.0f, 0.0f, zRotation },
                        { 1.0f, 1.0f, 1.0f }),
                camera);
        renderer.End();

        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
        auto pos = Input::GetMousePosition();

        static glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
        ImGui::Begin("Hello, world!");

        ImVec2 vMin = ImGui::GetWindowContentRegionMin();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        vMin.x += ImGui::GetWindowPos().x;
        vMin.y += ImGui::GetWindowPos().y;
        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        auto imagePos = glm::vec2(pos.x - vMin.x, pos.y - vMin.y);

        bool isHovered =
                imagePos.x >= 0 && imagePos.y >= 0 && imagePos.x < (float)image.GetWidth()
                        && imagePos.y < (float)image.GetHeight();

        if (isHovered && Input::GetButtonDown(GLFW_MOUSE_BUTTON_LEFT))
        {
            color = texture.GetPixel((int)imagePos.x, (int)imagePos.y);
        }
        ImGui::Image((ImTextureID)id, ImVec2((float)image.GetWidth(), (float)image.GetHeight()));

        ImGui::ColorEdit4("Color", &color.r);
        ImGui::Text("Screen position: (%.1f, %.1f)", pos.x, pos.y);
        if (isHovered)
            ImGui::Text("Image position: (%.1f, %.1f)", imagePos.x, imagePos.y);

        ImGui::End();
        ImGui::Render();
        ImGui::EndFrame();

        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            CheckVkResult((vkBeginCommandBuffer(renderer.GetGuiCommandBuffer(), &info)));
        }

        {
            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = renderer.GetGuiRenderPass();
            renderPassBeginInfo.framebuffer = renderer.GetCurrentGuiFramebuffer();
            renderPassBeginInfo.renderArea.extent.width = vulkanContext->GetSwapchainExtent().width;
            renderPassBeginInfo.renderArea.extent.height = vulkanContext->GetSwapchainExtent().height;
            VkClearValue clearValues[] = {
                    { 0.1f, 0.1f, 0.1f, 1.0f }};
            renderPassBeginInfo.pClearValues = clearValues;
            renderPassBeginInfo.clearValueCount = 1;
            vkCmdBeginRenderPass(renderer.GetGuiCommandBuffer(), &renderPassBeginInfo,
                    VK_SUBPASS_CONTENTS_INLINE);
        }

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderer.GetGuiCommandBuffer());

        {
            vkCmdEndRenderPass(renderer.GetGuiCommandBuffer());
            CheckVkResult(vkEndCommandBuffer(renderer.GetGuiCommandBuffer()));
        }

        renderer.EndFrame();
    }
    renderer.Shutdown();

    vulkanContext->Cleanup();

    ImGuiShutdown(imGuiContext);
    return 0;
}
