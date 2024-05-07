#include "Log.h"
#include "App.h"
#include "Input.h"
#include "Layer.h"

#include <string>
#include <GLFW/glfw3.h>
#include <RenderStats.h>

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
            for (auto layer : m_LayerStack)
            {
                if (!layer->m_Enabled)
                    continue;
                layer->OnEvent(Figment::AppEvent::WindowResize, (void *)&eventData);
            }
        });

        auto webGpuWindow = std::dynamic_pointer_cast<WebGPUWindow>(App::Instance()->GetWindow());
        m_RenderContext = webGpuWindow->GetContext();

        Input::Initialize((GLFWwindow *)m_Window->GetNative());
        m_GUICtx = std::make_unique<WebGPUGUIContext>();
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
        m_RenderContext->BeginFrame();
        RenderStats::Reset();

        m_CurrentTime = (float)glfwGetTime();
        float deltaTime = m_CurrentTime - m_LastTime;
        m_LastTime = m_CurrentTime;
        m_TimeSinceStart += deltaTime;

        m_FPSCounter.Update(deltaTime);

        ImGuiIO &io = ImGui::GetIO();
        if (m_InputEnabled && (!io.WantCaptureKeyboard || !io.WantCaptureMouse))
        {
            Input::Update();
        }

        for (auto layer : m_LayerStack)
        {
            if (!layer->m_Enabled)
                continue;
            layer->OnUpdate(deltaTime);
        }

        m_RenderContext->EndFrame();

        m_GUICtx->Begin();
        for (auto layer : m_LayerStack)
        {
            if (!layer->m_Enabled)
                continue;
            layer->OnImGuiRender();
        }
        m_GUICtx->Render();

        glfwPollEvents();
    }

    void App::AddLayer(Layer *layer)
    {
        Log::Info("Adding layer: %s", layer->GetName().c_str());
        m_LayerStack.AddLayer(layer);
        layer->OnAttach();
    }

    void App::AddOverlay(Layer *overlay)
    {
        Log::Info("Adding overlay: %s", overlay->GetName().c_str());
        m_LayerStack.AddOverlay(overlay);
        overlay->OnAttach();
    }
}
