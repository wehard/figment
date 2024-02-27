#include "SandSimulation.h"

SandSimulation::SandSimulation(Figment::WebGPUContext &context, Camera &camera) : Layer("SandSimulation"),
        m_Context(context), m_Camera(camera),
        m_Canvas(context, 320, 200)
{ }

void SandSimulation::OnAttach()
{

}
void SandSimulation::OnDetach()
{

}
void SandSimulation::OnUpdate(float deltaTime)
{

}
void SandSimulation::OnImGuiRender()
{

}
void SandSimulation::OnEvent(Figment::AppEvent event, void *eventData)
{

}
