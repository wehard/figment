#include "OrthoCamera.h"
#include <glm/gtc/matrix_transform.hpp>

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    m_ViewMatrix = glm::mat4(1.0);
}

OrthoCamera::OrthoCamera(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    Update();
}

void OrthoCamera::SetProjection(float left, float right, float bottom, float top)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void OrthoCamera::Update()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

    m_ViewMatrix = glm::inverse(transform);
}

void OrthoCamera::Zoom(float delta, glm::vec2 mousePosition)
{
    float zoomFactor = 1.0 + 0.05 * delta;
    m_Zoom *= zoomFactor;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    glm::vec2 zoomAt = glm::vec2((mousePosition.x / (1280 * 0.5)) - 1.0, (mousePosition.y / (720.0 * 0.5)) - 1.0);
    zoomAt.x *= -1.0;
    glm::vec3 newPosition = glm::vec3(0.0);
    newPosition.x = (m_Position.x - zoomAt.x) / zoomFactor + zoomAt.x;
    newPosition.y = (m_Position.y - zoomAt.y) / zoomFactor + zoomAt.y;
    newPosition.z = (m_Position.z - 0.0) / zoomFactor + 0.0;
    SetPosition(newPosition);
}

void OrthoCamera::OnResize(float width, float height)
{
    m_AspectRatio = width / height;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
}

void OrthoCamera::SetPosition(glm::vec3 position)
{
    m_Position = position;
    Update();
}
