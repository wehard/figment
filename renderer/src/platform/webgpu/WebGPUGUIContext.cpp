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

    WGPUTextureFormat textureFormat = WGPUTextureFormat_BGRA8Unorm;

    ImGui_ImplGlfw_InitForOther(glfwWindow, true);
    ImGui_ImplWGPU_Init(m_GfxContext->GetDevice(), 3, textureFormat, WGPUTextureFormat_Undefined);

    //

    ImGui_ImplWGPU_InvalidateDeviceObjects();
    WGPUSwapChainDescriptor swap_chain_desc = {};
    swap_chain_desc.usage = WGPUTextureUsage_RenderAttachment;
    swap_chain_desc.format = textureFormat;
    swap_chain_desc.width = window->GetFramebufferWidth();
    swap_chain_desc.height = window->GetFramebufferHeight();
    swap_chain_desc.presentMode = WGPUPresentMode_Fifo;
    m_SwapChain = wgpuDeviceCreateSwapChain(m_GfxContext->GetDevice(), m_GfxContext->GetSurface(), &swap_chain_desc);
    ImGui_ImplWGPU_CreateDeviceObjects();

    printf("WebGPUGUIContext created\n");
}

void WebGPUGUIContext::Render()
{
    ImGui::Render();
    WGPURenderPassColorAttachment color_attachments = {};
    color_attachments.loadOp = WGPULoadOp_Clear;
    color_attachments.storeOp = WGPUStoreOp_Store;
    color_attachments.clearValue = { 0.1, 0.1, 0.1, 1.0 };
    color_attachments.view = wgpuSwapChainGetCurrentTextureView(m_SwapChain);
    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachments;
    render_pass_desc.depthStencilAttachment = nullptr;

    WGPUCommandEncoderDescriptor enc_desc = {};
    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(m_GfxContext->GetDevice(), &enc_desc);

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor cmd_buffer_desc = {};
    WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
    WGPUQueue queue = wgpuDeviceGetQueue(m_GfxContext->GetDevice());
    wgpuQueueSubmit(queue, 1, &cmd_buffer);
}

void WebGPUGUIContext::Shutdown()
{
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(m_Context);
}