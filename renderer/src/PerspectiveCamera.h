#pragma once

#include "Camera.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 30.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class PerspectiveCamera : public Camera
{
private:
    float m_AspectRatio = 1.0;
    float m_Fov = 45.0;
    float m_NearClip = 0.1;
    float m_FarClip = 1000.0;
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;

public:
    glm::vec3 m_Position;
    glm::vec3 m_Forward;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;

    PerspectiveCamera(float aspectRatio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    ~PerspectiveCamera() override;

    void Update() override;
    void Resize(float width, float height) override;
    glm::mat4 GetViewMatrix() override { return m_ViewMatrix; };
    glm::mat4 GetProjectionMatrix() override { return m_ProjectionMatrix; }
    glm::vec3 GetPosition() override { return m_Position; }
    float GetAspectRatio() override { return m_AspectRatio; };

    void UpdateProjectionMatrix();
    void UpdateViewMatrix();


    glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize) override;
};
