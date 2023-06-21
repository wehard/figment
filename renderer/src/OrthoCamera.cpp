#include "OrthoCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

OrthoCamera::OrthoCamera()
{
}

OrthoCamera::OrthoCamera(float width, float height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    UpdateViewMatrix();
}

void OrthoCamera::SetProjection(float left, float right, float bottom, float top)
{
    m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void OrthoCamera::UpdateViewMatrix()
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
    UpdateViewMatrix();
}

void OrthoCamera::OnResize(float w, float h)
{
    m_ViewportWidth = w;
    m_ViewportHeight = h;
    m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    UpdateViewMatrix();
}

void OrthoCamera::OnUpdate(glm::vec2 mousePosition)
{
    if (m_IsPanning)
    {
        glm::vec2 mw = ScreenToWorldSpace(mousePosition);
        glm::vec2 delta = (mw - m_InitialMousePosition);

        glm::vec3 newPosition;
        newPosition.x = m_Position.x - delta.x;
        newPosition.y = m_Position.y - delta.y;

        SetPosition(newPosition);
    }
}

void OrthoCamera::SetPosition(glm::vec3 position)
{
    m_Position = position;
    UpdateViewMatrix();
}

glm::vec3 OrthoCamera::ScreenToWorldSpace(glm::vec2 screenPosition)
{
    double x = 2.0 * screenPosition.x / m_ViewportWidth - 1;
    double y = 2.0 * screenPosition.y / m_ViewportHeight - 1;

    glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);
    glm::mat4 ProjectionViewMatrix = GetProjectionMatrix() * GetViewMatrix();
    glm::mat4 InverseProjectionViewMatrix = glm::inverse(ProjectionViewMatrix);
    glm::vec4 worldPos = InverseProjectionViewMatrix * screenPos;

    return glm::vec3(worldPos.x, worldPos.y, worldPos.z);
}

void OrthoCamera::BeginPan(glm::vec2 mousePosition)
{
    m_IsPanning = true;
    glm::vec3 world = ScreenToWorldSpace(mousePosition);
    m_InitialMousePosition = glm::vec2(world.x, world.y);
}

void OrthoCamera::EndPan()
{
    m_IsPanning = false;
}

void OrthoCamera::SetZoom(float amount)
{
    m_Zoom = amount;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
    UpdateViewMatrix();
}
