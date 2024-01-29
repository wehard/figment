#include "Particles.h"

Particles::Particles(SharedPtr<PerspectiveCamera> camera) : Layer("Particles"), m_Camera(camera)
{
    auto m_Window = Figment::App::Instance()->GetWindow();
    auto webGpuWindow = std::dynamic_pointer_cast<Figment::WebGPUWindow>(m_Window);
    m_Renderer = Figment::CreateUniquePtr<Figment::WebGPURenderer>(*webGpuWindow->GetContext());
}

Particles::~Particles()
{

}

void Particles::OnAttach()
{
    FIG_LOG_INFO("Particles layer attached");
}

void Particles::OnDetach()
{

}

void Particles::OnUpdate(float deltaTime)
{
    m_Renderer->Begin(*m_Camera);
    m_Renderer->SubmitCircle(glm::vec3(0.0), glm::vec3(2.0), glm::vec4(1.0), 42);
    m_Renderer->End();
}

void Particles::OnImGuiRender()
{

}

void Particles::OnEvent(AppEvent event, void *eventData)
{
    auto ev = (Figment::WindowResizeEventData *)eventData;
    m_Renderer->OnResize(ev->Width, ev->Height);
}
