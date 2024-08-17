#include "Application.h"
#include "Log.h"
#include "Input.h"
#include "RenderStats.h"

namespace Figment
{
    Application::Application(const Application::Descriptor &&descriptor)
    {
        Log::Init();
        m_Window = Window::Create(descriptor.Name, descriptor.Width, descriptor.Height);
        Input::Initialize((GLFWwindow *)m_Window->GetNative());

        m_Window->SetResizeEventCallback([this](Window::ResizeEventData eventData)
        {
            for (auto layer : m_LayerStack)
            {
                if (!layer->m_Enabled)
                    continue;
                layer->OnEvent(Figment::AppEvent::WindowResize, (void *)&eventData);
            }
        });

        Log::Info("Application created");
    }

    void Application::Update()
    {
        glfwPollEvents();
        Input::Update();
        RenderStats::Reset();

        m_CurrentTime = (float)glfwGetTime();
        float deltaTime = m_CurrentTime - m_LastTime;
        m_LastTime = m_CurrentTime;
        m_TimeSinceStart += deltaTime;

        m_FPSCounter.Update(deltaTime);

        for (auto layer : m_LayerStack)
        {
            if (!layer->m_Enabled)
                continue;
            layer->OnUpdate(deltaTime);
        }
    }

    void Application::AddLayer(Layer *layer)
    {
        Log::Info("Adding layer: %s", layer->GetName().c_str());
        m_LayerStack.AddLayer(layer);
        layer->OnAttach();
    }

    void Application::AddOverlay(Layer *overlay)
    {
        Log::Info("Adding overlay: %s", overlay->GetName().c_str());
        m_LayerStack.AddOverlay(overlay);
        overlay->OnAttach();
    }

    void Application::Start()
    {
        for (auto layer : m_LayerStack)
        {
            if (!layer->m_Enabled)
                continue;
            layer->OnEnable();
        }

        while (!m_Window->ShouldClose() && !Input::GetKey(GLFW_KEY_ESCAPE))
        {
            Update();
        }

        for (auto layer : m_LayerStack)
        {
            if (!layer->m_Enabled)
                continue;
            layer->OnDisable();
        }
    }
}
