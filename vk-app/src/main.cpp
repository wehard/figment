#include "Window.h"
#include "VulkanContext.h"
#include "Log.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

using namespace Figment;

int main()
{
    Log::Init();
    auto window = Figment::Window::Create("Figment", 1280, 720);

    auto context = ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)window->GetNative(), true);
    auto vkContext = window->GetContext<VulkanContext>();
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Device = vkContext->GetDevice();
    initInfo.Instance = vkContext->GetInstance();
    initInfo.PhysicalDevice = vkContext->GetPhysicalDevice();
    initInfo.Queue = vkContext->GetGraphicsQueue();
    // initInfo.QueueFamily = g_QueueFamily;
    // initInfo.PipelineCache = g_PipelineCache;
    // initInfo.DescriptorPool = g_DescriptorPool;
    // initInfo.RenderPass = wd->RenderPass;
    // initInfo.Subpass = 0;
    // initInfo.MinImageCount = g_MinImageCount;
    // initInfo.ImageCount = wd->ImageCount;
    // initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    // initInfo.Allocator = nullptr;
    // initInfo.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&initInfo);

    while (!window->ShouldClose())
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkan_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();
        ImGui::Render();

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), nullptr);
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(context);
    return 0;
}
