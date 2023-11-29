#include "WebGPUGUIContext.h"
#include "Log.h"
#include "WebGPUWindow.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"

namespace Figment
{
    void WebGPUGUIContext::Init(std::shared_ptr<Window> window, const char *glslVersion)
    {
        IMGUI_CHECKVERSION();
        m_Context = ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
        // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
        io.IniFilename = NULL;

        ImGui::StyleColorsDark();

        auto *glfwWindow = (GLFWwindow *)window->GetNative();
        auto webGpuWindow = std::dynamic_pointer_cast<WebGPUWindow>(window);
        m_GfxContext = webGpuWindow->GetContext().get();

        ImGui_ImplGlfw_InitForOther(glfwWindow, true);
        ImGui_ImplWGPU_Init(m_GfxContext->GetDevice(), 3, m_GfxContext->GetTextureFormat(),
                WGPUTextureFormat_Undefined);

        ImGui_ImplWGPU_InvalidateDeviceObjects();
        ImGui_ImplWGPU_CreateDeviceObjects();

        FIG_LOG_INFO("WebGPU GUIContext initialized");
    }

    void WebGPUGUIContext::Render()
    {
        WGPUCommandEncoderDescriptor desc = {};
        auto commandEncoder = wgpuDeviceCreateCommandEncoder(m_GfxContext->GetDevice(), &desc);

        WGPURenderPassColorAttachment colorAttachment = {};

        colorAttachment.loadOp = WGPULoadOp_Load;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = { 0.0, 0.0, 0.0, 0.0 };
        colorAttachment.view = wgpuSwapChainGetCurrentTextureView(m_GfxContext->GetSwapChain());

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;
        renderPassDesc.depthStencilAttachment = nullptr;

        auto renderPass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass);

        wgpuRenderPassEncoderEnd(renderPass);

        WGPUCommandBufferDescriptor commandBufferDesc = {};
        WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDesc);
        WGPUQueue queue = wgpuDeviceGetQueue(m_GfxContext->GetDevice());
        wgpuQueueSubmit(queue, 1, &commandBuffer);

        wgpuCommandEncoderRelease(commandEncoder);
        wgpuRenderPassEncoderRelease(renderPass);
        wgpuCommandBufferRelease(commandBuffer);
    }

    void WebGPUGUIContext::Shutdown()
    {
        ImGui_ImplWGPU_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext(m_Context);
    }
}
