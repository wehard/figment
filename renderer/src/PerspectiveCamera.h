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
    // euler Angles
    float m_Yaw;
    float m_Pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float m_Zoom;

    PerspectiveCamera(float aspectRatio, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    ~PerspectiveCamera();

    void UpdateProjectionMatrix();
    void UpdateViewMatrix();

    void OnUpdate(glm::vec2 mp);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    void Move(CameraDirection direction, float deltaTime);
    void Reset(glm::vec3 position, float yaw, float pitch);
    void BeginPan(glm::vec2 mousePosition);
    void EndPan();
    void Zoom(float delta, glm::vec2 mousePosition);
    void Rotate(float xoffset, float yoffset, bool constrainPitch = true);
    void SetViewportSize(float width, float height);

    glm::vec3 GetPosition() { return m_Position; }
    void SetPosition(glm::vec3 position);
    float GetZoom() { return m_Zoom; }
    void SetZoom(float amount);
    float GetAspectRatio() { return m_AspectRatio; };
    glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition, glm::vec2 viewportSize);
    void OnResize(float width, float height);
};
