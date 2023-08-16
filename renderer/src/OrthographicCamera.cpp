#include "OrthographicCamera.h"
#include "Input.h"
#include <glm/gtc/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera()
{
}

OrthographicCamera::OrthographicCamera(float width, float height) : m_ViewportWidth(width), m_ViewportHeight(height), m_AspectRatio(m_ViewportWidth / m_ViewportHeight)
{
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    UpdateViewMatrix();
}

void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void OrthographicCamera::UpdateViewMatrix()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

    m_ViewMatrix = glm::inverse(transform);
}

//void OrthographicCamera::Zoom(float delta, glm::vec2 mousePosition)
//{
//    float zoomFactor = 1.0 + 0.05 * delta;
//    m_Zoom *= zoomFactor;
//    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
//    UpdateViewMatrix();
//}

void OrthographicCamera::Resize(float w, float h)
{
    m_ViewportWidth = w;
    m_ViewportHeight = h;
    m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    UpdateViewMatrix();
}

void OrthographicCamera::Update()
{
    if (m_IsPanning)
    {
        auto mousePosition = Input::GetMousePosition();
        glm::vec3 mw = ScreenToWorldSpace(mousePosition, glm::vec2(m_ViewportWidth, m_ViewportHeight));
        glm::vec2 delta = (glm::vec2(mw.x, mw.y) - m_InitialMousePosition);

        glm::vec3 newPosition;
        newPosition.x = m_Position.x - delta.x;
        newPosition.y = m_Position.y - delta.y;

        m_Position = newPosition;
    }
}

glm::vec3 OrthographicCamera::ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize)
{
    double x = 2.0 * screenPosition.x / viewportSize.x - 1;
    double y = 2.0 * screenPosition.y / viewportSize.y - 1;

    glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);
    glm::mat4 ProjectionViewMatrix = GetProjectionMatrix() * GetViewMatrix();
    glm::mat4 InverseProjectionViewMatrix = glm::inverse(ProjectionViewMatrix);
    glm::vec4 worldPos = InverseProjectionViewMatrix * screenPos;

    return {worldPos.x, worldPos.y, 0.0f};
}

//void OrthographicCamera::BeginPan(glm::vec2 mousePosition)
//{
//    m_IsPanning = true;
//    glm::vec3 world = ScreenToWorldSpace(mousePosition, glm::vec2(m_ViewportWidth, m_ViewportHeight));
//    m_InitialMousePosition = glm::vec2(world.x, world.y);
//}
//
//void OrthographicCamera::EndPan()
//{
//    m_IsPanning = false;
//}
