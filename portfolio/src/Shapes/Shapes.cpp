#include "Shapes.h"

Shapes::Shapes(WebGPUContext &context, PerspectiveCamera &camera) : Layer("Shapes", true), m_Context(context),
        m_Camera(camera),
        m_ShapeRenderer(context)
{
    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 10; x++)
        {
            m_Points.emplace_back(x, y, 0);
        }
    }
}

void Shapes::OnAttach()
{

}

void Shapes::OnDetach()
{

}

void Shapes::OnEnable()
{
    m_Camera.SetPosition(glm::vec3(0.0, 0.0, 20.0));
}

void Shapes::OnDisable()
{

}

void Shapes::OnUpdate(float deltaTime)
{
    static glm::vec3 offset = { -4.5f, -4.5f, 0.0f };
    m_ShapeRenderer.Begin(m_Camera);
    for (auto &point : m_Points)
    {
        m_ShapeRenderer.SubmitCircle(point + offset, { 0.8f, 0.2f, 0.3f, 1.0f }, 1.0, -1);
    }
    m_ShapeRenderer.End();
}

void Shapes::OnImGuiRender()
{

}

void Shapes::OnEvent(AppEvent event, void *eventData)
{

}
