#include "Shapes.h"

Shapes::Shapes(WebGPUContext &context, PerspectiveCamera &camera) : Layer("Shapes", true), m_Context(context),
        m_Camera(camera),
        m_ShapeRenderer(context)
{
}

void Shapes::OnAttach()
{

}

void Shapes::OnDetach()
{

}

void Shapes::OnUpdate(float deltaTime)
{
    m_ShapeRenderer.Begin(m_Camera);
    m_ShapeRenderer.SubmitQuad({ -1.0f, 0.0f, 0.0f }, { 0.8f, 0.2f, 0.3f, 1.0f }, -1);
    m_ShapeRenderer.SubmitCircle({ 1.0f, 0.0f, 0.0f }, { 0.8f, 0.2f, 0.3f, 1.0f }, 1.0, -1);
    m_ShapeRenderer.End();
}

void Shapes::OnImGuiRender()
{

}

void Shapes::OnEvent(AppEvent event, void *eventData)
{

}
