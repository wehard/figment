#include "application.h"
#include "Input.h"
#include "RenderStats.h"

#include <spdlog/spdlog.h>
#include <BaseWindow.h>

namespace figment
{
Application::Application(const Descriptor&& descriptor):
    window(descriptor.Name, descriptor.Width, descriptor.Height), imguiRenderer(window)
{
    Input::Initialize(window.GetNative());

    window.SetResizeEventCallback(
        [this](Window::ResizeEventData eventData)
        {
            for (auto layer: m_LayerStack)
            {
                if (!layer->m_Enabled)
                    continue;
                layer->OnEvent(AppEvent::WindowResize, (void*)&eventData);
            }
        });
    spdlog::info("Application created");
}

void Application::Update()
{
    Input::Update();
    RenderStats::Reset();

    m_CurrentTime   = (float)glfwGetTime();
    float deltaTime = m_CurrentTime - m_LastTime;
    m_LastTime      = m_CurrentTime;
    m_TimeSinceStart += deltaTime;

    m_FPSCounter.Update(deltaTime);

    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnUpdate(deltaTime);
    }
    window.nextImage();

    imguiRenderer.beginFrame();
    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnImGuiRender();
    }
    imguiRenderer.endFrame();

    window.present();

    glfwPollEvents();
}

void Application::AddLayer(Layer* layer)
{
    spdlog::info("Adding layer: {}", layer->GetName());
    m_LayerStack.AddLayer(layer);
    layer->OnAttach();
}

void Application::AddOverlay(Layer* overlay)
{
    spdlog::info("Adding overlay: {}", overlay->GetName());
    m_LayerStack.AddOverlay(overlay);
    overlay->OnAttach();
}

void Application::Start()
{
    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnEnable();
    }

    while (!window.ShouldClose() && !Input::GetKey(GLFW_KEY_ESCAPE))
    {
        Update();
    }

    for (auto layer: m_LayerStack)
    {
        if (!layer->m_Enabled)
            continue;
        layer->OnDisable();
    }
}
} // namespace figment
