#include "OrthoCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

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

void OrthoCamera::BeginPan(glm::vec2 mousePosition)
{
    // m_InitialMousePosition = mousePosition;
    // m_IsPanning = true;
}

void OrthoCamera::EndPan()
{
    // m_IsPanning = false;
}

void OrthoCamera::Pan(glm::vec2 mousePosition)
{
    glm::vec2 mw = ScreenToWorldSpace(mousePosition.x, mousePosition.y);
    glm::vec2 delta = (mw - m_InitialMousePosition) * 0.5f;

    float x = std::min(m_ViewportSize.x / 1000.0f, 2.4f); // max = 2.4f
    float xSpeed = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

    float y = std::min(m_ViewportSize.y / 1000.0f, 2.4f); // max = 2.4f
    float ySpeed = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

    glm::vec3 newPosition;
    newPosition.x = m_Position.x - delta.x;
    newPosition.y = m_Position.y - delta.y;

    SetPosition(newPosition);
}

void OrthoCamera::OnResize(float width, float height)
{
    m_ViewportSize.x = width;
    m_ViewportSize.y = height;
    m_AspectRatio = width / height;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
}

void OrthoCamera::SetPosition(glm::vec3 position)
{
    m_Position = position;
    Update();
}

glm::vec2 OrthoCamera::ScreenToWorldSpace(int mouseX, int mouseY)
{
    double x = 2.0 * mouseX / m_ViewportSize.x - 1;
    double y = 2.0 * mouseY / m_ViewportSize.y - 1;

    glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);
    glm::mat4 ProjectionViewMatrix = GetProjectionMatrix() * GetViewMatrix();
    glm::mat4 InverseProjectionViewMatrix = glm::inverse(ProjectionViewMatrix);
    glm::vec4 worldPos = InverseProjectionViewMatrix * screenPos;

    return glm::vec2(worldPos.x, worldPos.y);
}
