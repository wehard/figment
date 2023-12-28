#include "Core.h"
#include "Log.h"
#include "App.h"
#include "Input.h"

#include <string>
#include <GLFW/glfw3.h>

#include "imgui_impl_wgpu.h"
#include "WebGPUWindow.h"

namespace Figment
{
    App *App::s_Instance = nullptr;

    App::App(uint32_t width, uint32_t height)
    {
        s_Instance = this;
        Log::Init();
        m_Window = Window::Create("Figment C++", width, height);
        m_Window->SetResizeEventCallback([this](WindowResizeEventData eventData)
        {
            for (auto &layer : m_Layers)
            {
                layer->OnEvent(Figment::AppEvent::WindowResize, (void *)&eventData);
            }
        });

        Input::Initialize((GLFWwindow *)m_Window->GetNative());
        m_GUICtx = Figment::CreateUniquePtr<WebGPUGUIContext>();
        m_GUICtx->Init(m_Window, "glslVersion");

        FIG_LOG_INFO("App initialized");
    }

    App::~App()
    {
        m_GUICtx->Shutdown();
        // Log::Shutdown();
    }

    void App::Update()
    {
        m_CurrentTime = (float)glfwGetTime();
        float deltaTime = m_CurrentTime - m_LastTime;
        m_LastTime = m_CurrentTime;
        m_TimeSinceStart += deltaTime;

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

        m_GUICtx->Begin();
        for (auto &layer : m_Layers)
        {
            layer->OnImGuiRender();
        }
        m_GUICtx->Render();

        glfwPollEvents();
    }
}
