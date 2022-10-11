#pragma once

#include <glm/glm.hpp>

class OrthoCamera
{
public:
    OrthoCamera(float left, float right, float bottom, float top);
    OrthoCamera(float aspectRatio);

    void SetProjection(float left, float right, float bottom, float top);
    void SetPosition(glm::vec3 position);
    void Update();
    void Zoom(float delta, glm::vec2 mousePosition);
    void Pan(glm::vec2 mousePosition);
    void OnResize(float width, float height);
    glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }
    glm::mat4 GetViewMatrix() { return m_ViewMatrix; }
    glm::vec3 GetPosition() { return m_Position; }
    float GetZoom() { return m_Zoom; }
    glm::vec2 ScreenToWorldSpace(int mouseX, int mouseY);

    void BeginPan(glm::vec2 mousePosition);
    void EndPan();
    bool m_IsPanning = false;

private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

    glm::vec2 m_ViewportSize = {1280, 720};
    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
    float m_Rotation = 0.0f;
    float m_AspectRatio = 1.0f;
    float m_Zoom = 1.0f;
    glm::vec2 m_InitialMousePosition = {0.0f, 0.0f};
};
