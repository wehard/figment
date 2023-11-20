#pragma once

#include "Camera.h"
#include <glm/glm.hpp>

class OrthographicCamera : public Figment::Camera
{
public:
    OrthographicCamera();
    OrthographicCamera(float width, float height);

    void Update() override;
    void Resize(float width, float height) override;
    glm::mat4 GetProjectionMatrix() override { return m_ProjectionMatrix; }
    glm::mat4 GetViewMatrix() override { return m_ViewMatrix; }
    float GetAspectRatio() override { return m_AspectRatio; };
    glm::vec3 GetPosition() override { return m_Position; }
    glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) override;

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
