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
        auto eventData = Figment::WindowResizeEventData { (uint32_t)data.FramebufferWidth,
                                                          (uint32_t)data.FramebufferHeight };
        for (auto &layer : m_Layers)
        {
            layer->OnEvent(Figment::AppEvent::WindowResize, (void *)&eventData);
        }
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
