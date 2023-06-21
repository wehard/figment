#pragma once

#include "Camera.h"
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 15.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class PerspectiveCamera : public Camera
{
private:
    float fov;
    glm::mat4x4 projection_matrix;
    glm::vec3 initPosition;

public:
    glm::vec3 m_Position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float m_Zoom;

    PerspectiveCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    ~PerspectiveCamera();
    void OnUpdate(glm::vec2 mp);
    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    void Move(CameraDirection direction, float deltaTime);
    void Rotate(float xoffset, float yoffset, bool constrainPitch = true);
    void Reset(glm::vec3 position, float yaw, float pitch);
    void BeginPan(glm::vec2 mousePosition);
    void EndPan();
    void Zoom(float delta, glm::vec2 mousePosition);

    glm::vec3 GetPosition() { return m_Position; }
    void SetPosition(glm::vec3 position);
    float GetZoom() { return m_Zoom; }
    void SetZoom(float amount);
    float GetAspectRatio() { return 1.777; };
    glm::vec3 ScreenToWorldSpace(glm::vec2 screenPosition);
    void OnResize(float width, float height);
};
