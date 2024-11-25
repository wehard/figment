#include <spdlog/spdlog.h>
#include "Application.h"
#include "Input.h"
#include "RenderStats.h"

namespace Figment
{
    Application::Application(const Application::Descriptor &&descriptor)
    {
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

        spdlog::info("Application created");
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
        spdlog::info("Adding layer: {}", layer->GetName());
        m_LayerStack.AddLayer(layer);
        layer->OnAttach();
    }

    void Application::AddOverlay(Layer *overlay)
    {
        spdlog::info("Adding overlay: {}", overlay->GetName());
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
