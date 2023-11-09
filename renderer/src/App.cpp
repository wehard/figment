#include "Core.h"
#include "App.h"
#include "Input.h"

#include <string>
#include <GLFW/glfw3.h>

#include "imgui_impl_wgpu.h"
#include "WebGPUWindow.h"
#include "EditorLayer.h"

App *App::s_Instance = nullptr;

App::App(uint32_t width, uint32_t height)
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

    Input::Initialize((GLFWwindow *)m_Window->GetNative());
    m_GUICtx = std::make_unique<WebGPUGUIContext>();
    m_GUICtx->Init(m_Window, "glslVersion");

    m_Layers.emplace_back(std::make_unique<Figment::EditorLayer>());
}

App::~App()
{
    m_GUICtx->Shutdown();
}

void App::Update()
{
    m_CurrentTime = (float)glfwGetTime();
    float deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime = m_CurrentTime;

    m_FPSCounter.Update(deltaTime);

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
