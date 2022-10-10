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

void OrthoCamera::Zoom(float delta)
{
    m_Zoom *= 1.0 + 0.05 * delta;
    SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
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
