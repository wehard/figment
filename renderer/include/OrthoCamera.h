#pragma once

#include <glm/glm.hpp>

class OrthoCamera
{
public:
    OrthoCamera(float left, float right, float bottom, float top);
    OrthoCamera(float aspectRatio);

    void SetProjection(float left, float right, float bottom, float top);
    glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }
    glm::mat4 GetViewMatrix() { return m_ViewMatrix; }
    glm::vec3 GetPosition() { return m_Position; }
    void SetPosition(glm::vec3 position);
    float GetZoom() { return m_Zoom; }
    void Update();
    void Zoom(float delta);
    void OnResize(float width, float height);

private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
    float m_Rotation = 0.0f;

    float m_AspectRatio = 1.0f;
    float m_Zoom = 1.0f;
};
