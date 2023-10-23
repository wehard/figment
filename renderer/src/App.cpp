#include "Core.h"
#include "App.h"
#include "Input.h"

#include <string>
#include <GLFW/glfw3.h>

#include "imgui_impl_wgpu.h"
#include "WebGPUWindow.h"
#include "ImGuiLayer.h"

App *App::s_Instance = nullptr;

App::App(float width, float height)
{
    s_Instance = this;
    m_Window = Window::Create("Figment C++", width, height);
    m_Window->SetResizeEventCallback([this](WindowResizeEventData data)
    {
        OnResize(data.FramebufferWidth, data.FramebufferHeight);
    });

    auto *glfwWindow = (GLFWwindow *)m_Window->GetNative();
    Input::Initialize(glfwWindow);
    m_GUICtx = std::make_unique<WebGPUGUIContext>();

    const char *glslVersion = "#version 300 es";

    m_GUICtx->Init(m_Window, glslVersion);

    m_Layers.emplace_back(std::make_unique<Figment::ImGuiLayer>());
}

App::~App()
{
    m_GUICtx->Shutdown();
}

void App::Update()
{
    m_CurrentTime = glfwGetTime();
    double deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime = m_CurrentTime;

    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureKeyboard || !io.WantCaptureMouse)
    {
        Input::Update();
    }

    for (auto &layer : m_Layers)
    {
        layer->OnUpdate(deltaTime);
    }

    for (auto &layer : m_Layers)
    {
        layer->OnImGuiRender();
    }
    m_GUICtx->Render();

    glfwPollEvents();
}


void App::OnResize(uint32_t width, uint32_t height)
{
    // m_Scene->OnResize(width, height);
    // m_Renderer->OnResize(width, height);
    Update(); // TODO: Figure out if there is a better way
}
