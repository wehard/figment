#include "WebGPUGUIContext.h"
#include "WebGPUWindow.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"

void WebGPUGUIContext::Init(std::shared_ptr<Window> window, const char *glslVersion)
{
    IMGUI_CHECKVERSION();
    m_Context = ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    ImGui::StyleColorsDark();

    auto *glfwWindow = (GLFWwindow*)window->GetNative();
    auto webGpuWindow = std::dynamic_pointer_cast<WebGPUWindow>(window);
    m_GfxContext = webGpuWindow->GetContext().get();

    ImGui_ImplGlfw_InitForOther(glfwWindow, true);
    ImGui_ImplWGPU_Init(m_GfxContext->GetDevice(), 3, m_GfxContext->GetTextureFormat(), WGPUTextureFormat_Undefined);

    ImGui_ImplWGPU_InvalidateDeviceObjects();
    ImGui_ImplWGPU_CreateDeviceObjects();

    printf("WebGPUGUIContext created\n");
    m_Renderer = new WebGPURenderer(*m_GfxContext);
}

void WebGPUGUIContext::Render()
{
    ImGui::Render();
    auto pass = m_Renderer->Begin();
    m_Renderer->DrawQuad({}, {});
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);

    m_Renderer->End();
}

void WebGPUGUIContext::Shutdown()
{
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(m_Context);
}