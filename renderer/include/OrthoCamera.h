#pragma once

#include "Camera.h"
#include <glm/glm.hpp>

class OrthoCamera : public Camera
{
public:
    OrthoCamera();
    OrthoCamera(float width, float height);

    void SetPosition(glm::vec3 position);
    glm::vec3 GetPosition() { return m_Position; }
    void SetZoom(float amount);
    float GetZoom() { return m_Zoom; }
    void OnUpdate(glm::vec2 mousePosition);
    void OnResize(float width, float height);
    glm::mat4 GetProjectionMatrix() { return m_ProjectionMatrix; }
    glm::mat4 GetViewMatrix() { return m_ViewMatrix; }
    float GetAspectRatio() { return m_AspectRatio; };
    glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition);
    void Zoom(float delta, glm::vec2 mousePosition);
    void BeginPan(glm::vec2 mousePosition);
    void EndPan();
    void Move(CameraDirection direction, float deltaTime);

private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};

    float m_ViewportWidth;
    float m_ViewportHeight;
    float m_AspectRatio;
    float m_Zoom = 1.0f;
    float m_Rotation = 0.0f;
    bool m_IsPanning = false;
    glm::vec2 m_InitialMousePosition = {0.0f, 0.0f};

    void SetProjection(float left, float right, float bottom, float top);
    void UpdateViewMatrix();
};
